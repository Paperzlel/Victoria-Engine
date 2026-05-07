#include "platform/linux/file_handle_linux.h"

void FileHandleLinux::close() {
	if (stream) {
		fclose(stream);
		stream = nullptr;
	}
}

FileHandleLinux::FileHandleLinux() {}

FileHandleLinux::~FileHandleLinux() {
	close();
}