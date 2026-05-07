#include "platform/linux/filesystem_linux.h"

#include "platform/linux/file_handle_linux.h"

#include <stdio.h>

Error FileSystemLinux::_open_file(const Ref<FileHandle> &p_handle) {
	char permissions[3];

	int flags = p_handle->get_flags();
	if (flags & FileHandle::FLAG_READ) {
		permissions[0] = 'r';
	}

	if (flags & FileHandle::FLAG_WRITE) {
		permissions[0] = 'w';
	}

	if ((flags & (FileHandle::FLAG_READ | FileHandle::FLAG_WRITE)) == 3) {
		permissions[1] = '+';
	}

	FILE *fptr = fopen(p_handle->get_path(), permissions);
	ERR_COND_NULL_R(fptr, ERR_CANT_LOAD);
	Ref<FileHandleLinux> fh = p_handle;
	// This is invariably a bug with upwards casting.
	ERR_FAIL_COND_R(fh.is_valid(), ERR_BUG);
	fh->stream = fptr;
	return OK;
}
