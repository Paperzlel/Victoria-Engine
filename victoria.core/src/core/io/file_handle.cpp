#include "core/io/file_handle.h"

void FileHandle::set_flags(int p_flags) {
	if (p_flags & FLAG_READ && !(p_flags & FLAG_WRITE)) {
		read_only = true;
	}

	flags = p_flags;
}

int FileHandle::get_flags() const {
	return flags;
}

FileHandle::FileHandle() {}

FileHandle::~FileHandle() {}