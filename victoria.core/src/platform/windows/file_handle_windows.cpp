#include "platform/windows/file_handle_windows.h"
#if PLATFORM_WINDOWS

uint64_t FileHandleWindows::_get_of_size(int p_size) {
	if (!is_po2(p_size) || p_size > 8) {
		return 0;
	}

	uint64_t ret;
	DWORD actual_count = 0;
	if (!ReadFile(handle, &ret, p_size, &actual_count, nullptr)) {
		return 0; // NOTE: Should probably warn, this is unexpected.
	}

	ERR_FAIL_COND_MSG_R(actual_count != p_size, "Invalid number of bytes read.", 0);
	return ret;
}

Error FileHandleWindows::_open_internal(const String &p_path, int p_flags) {
	DWORD access_perms = 0;
	DWORD create_mode = 0;
	if (p_flags & FLAG_READ) {
		access_perms |= GENERIC_READ;
		create_mode = OPEN_EXISTING;
	}
	if (p_flags & FLAG_WRITE) {
		access_perms |= GENERIC_WRITE;
		create_mode = CREATE_ALWAYS;
	}

	String path = p_path;
	path.replace('/', '\\');

	HANDLE file = CreateFileA(path.ptr(), access_perms, 0, nullptr, create_mode, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (file == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		if (err == ERROR_FILE_NOT_FOUND || err == ERROR_PATH_NOT_FOUND) {
			return ERR_FILE_NOT_FOUND;
		}

		if (err == ERROR_ACCESS_DENIED) {
			return ERR_FILE_CANT_ACCESS;
		}

		if (err != ERROR_ALREADY_EXISTS || err != ERROR_FILE_EXISTS) {
			return ERR_FAILED; // generic failure reason
		}
	}

	if (length == -1) {
		DWORD extra = 0;
		length = GetFileSize(file, &extra);
		if (length == INVALID_FILE_SIZE) {
			length = -1;
		} else {
			if (extra > 0) {
				length += ((int64_t)extra << 32);
			}
		}
	}

	handle = file;
	return OK;
}

void FileHandleWindows::close() {
	ERR_COND_NULL_MSG(handle, "Cannot perform operations on a null file handle.");
	if (CloseHandle(handle)) {
		handle = nullptr;
		length = -1;
	}
}

void FileHandleWindows::seek(int p_position) {
	ERR_COND_NULL_MSG(handle, "Cannot perform operations on a null file handle.");

	DWORD pos = SetFilePointer(handle, p_position, nullptr, FILE_BEGIN);
	ERR_FAIL_COND_MSG(pos != p_position || pos == INVALID_SET_FILE_POINTER,
					  "File position has not been set properly.");
}

uint64_t FileHandleWindows::get_position() const {
	ERR_COND_NULL_MSG_R(handle, "Cannot perform operations on a null file handle.", 0);
	DWORD pos = SetFilePointer(handle, 0, 0, FILE_CURRENT);
	ERR_FAIL_COND_R(pos == INVALID_SET_FILE_POINTER, 0);
	return (uint64_t)pos;
}

uint64_t FileHandleWindows::get_length() const {
	return length;
}

int FileHandleWindows::get_buffer(uint8_t *p_buffer, int p_length) {
	ERR_COND_NULL_MSG_R(handle, "Cannot perform operations on a null file handle.", 0);
	DWORD read = 0;
	ERR_FAIL_COND_MSG_R(!ReadFile(handle, p_buffer, p_length, &read, nullptr), "Reading the file failed.", 0);
	ERR_FAIL_COND_R(read != p_length, 0);
	return read;
}

void FileHandleWindows::store_buffer(const uint8_t *p_buffer, int p_length) {
	ERR_COND_NULL_MSG(handle, "Cannot perform operations on a null file handle.");
	DWORD write = 0;
	ERR_FAIL_COND_MSG(!WriteFile(handle, p_buffer, p_length, &write, nullptr), "Writing to the file failed.");
	ERR_FAIL_COND(write != p_length);
}

#endif // PLATFORM_WINDOWS