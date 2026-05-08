#include "platform/linux/file_handle_linux.h"

#include <errno.h>
#include <sys/stat.h>

void FileHandleLinux::_write_generic(const uint8_t *p_buffer, int p_length) {
	ERR_FAIL_COND_MSG(!(get_flags() & FLAG_READ), "File does not have writing permissions.");

	int len = fwrite(p_buffer, 1, p_length, f);
	ERR_FAIL_COND_MSG(len < p_length, "Unable to write the full string into the file.");
}

uint64_t FileHandleLinux::_get_of_size(int p_size) {
	// Fail silently, this method isn't exposed and shouldn't need to fail
	if (!is_po2(p_size) || p_size > 8) {
		return 0;
	}

	uint64_t ret;
	fread(&ret, p_size, 1, f);
	return ret;
}

Error FileHandleLinux::_open_internal(const String &p_path, int p_flags) {
	path = p_path;
	const char *permissions = nullptr;

	int write_flags = p_flags & FLAG_WRITE_READ;
	if (write_flags == FLAG_READ) {
		permissions = "rb";
	} else if (write_flags == FLAG_WRITE) {
		permissions = "wb";
	} else if (write_flags == FLAG_READ_WRITE) {
		permissions = "rb+";
	} else if (write_flags == FLAG_WRITE_READ) {
		permissions = "wb+";
	}

	struct stat info;
	if (stat(p_path.get_data(), &info)) {
		length = info.st_size;
	}

	f = fopen(p_path.get_data(), permissions);
	if (f == nullptr) {
		switch (errno) {
			case ENOENT:
				return ERR_FILE_NOT_FOUND;
			default:
				return ERR_FILE_CANT_ACCESS;
		}
	}

	// Set file length if not found by stat()
	if (length == -1) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
	}

	return OK;
}

void FileHandleLinux::close() {
	if (f) {
		fclose(f);
		f = nullptr;
		length = -1;
	}
}

void FileHandleLinux::seek(int p_position) {
	fseek(f, p_position, SEEK_SET);
}

uint64_t FileHandleLinux::get_position() const {
	return ftell(f);
}

uint64_t FileHandleLinux::get_length() const {
	return length;
}

Vector<uint8_t> FileHandleLinux::get_buffer(int p_length) {
	Vector<uint8_t> ret;
	ret.resize(p_length);

	uint8_t *ptr = ret.ptrw();
	int len = get_buffer(ptr, p_length);
	if (len < p_length) {
		ret.resize(len);
	}

	return ret;
}

int FileHandleLinux::get_buffer(uint8_t *p_buffer, int p_length) {
	return fread(p_buffer, 1, p_length, f);
}

void FileHandleLinux::store_string(const String &p_string) {
	_write_generic((const uint8_t *)p_string.ptr(), p_string.length());
}

void FileHandleLinux::store_buffer(const Vector<uint8_t> &p_buffer) {
	_write_generic(p_buffer.ptr(), p_buffer.size());
}

FileHandleLinux::FileHandleLinux() {}

FileHandleLinux::~FileHandleLinux() {
	close();
}
