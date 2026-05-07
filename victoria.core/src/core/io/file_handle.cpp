#include "core/io/file_handle.h"

void FileHandle::set_flags(int p_flags) {
	flags = p_flags;
}

int FileHandle::get_flags() const {
	return flags;
}

bool FileHandle::is_eof() const {
	return get_position() >= get_length();
}

uint8_t FileHandle::get_8() {
	return (uint8_t)_get_of_size(1);
}

uint16_t FileHandle::get_16() {
	return (uint16_t)_get_of_size(2);
}

uint32_t FileHandle::get_32() {
	return (uint32_t)_get_of_size(4);
}

uint64_t FileHandle::get_64() {
	return (uint64_t)_get_of_size(8);
}

String FileHandle::get_line() {
	int offset = 0;
	int c = 0;
	while (c != -1) {
		if (c == (int)'\n') {
			break;
		}
		c = get_8();
		if (c != (int)'\r') {
			offset++;
		}
	}

	// Was only a newline char, return string. Don't need to offset the pointer.
	if (offset == 1) {
		return String();
	}

	String ret;
	ret.resize(offset); // offset is always size + 1 because of when the newline is detected
	int len = get_buffer((uint8_t *)ret.ptrw(), offset - 1);
	if (len < offset - 1) {
		ret.resize(len);
	}
	ret[offset - 1] = 0;
	return ret;
}

String FileHandle::get_as_string() {
	uint64_t size = get_length();
	String ret;
	ret.resize(size + 1);

	int len = get_buffer((uint8_t *)ret.ptrw(), size);
	ERR_FAIL_COND_MSG_R(len < size, String(), "Unable to read the full file.");
	seek(0);
	ret[size] = 0;
	return ret;
}

FileHandle::FileHandle() {}

FileHandle::~FileHandle() {}