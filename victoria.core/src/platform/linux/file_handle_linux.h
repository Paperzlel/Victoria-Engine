#pragma once

#include "core/io/file_handle.h"
#include "core/typedefs.h"

#include <stdio.h>

class FileHandleLinux : public FileHandle {
	VREGISTER_CLASS(FileHandleLinux, FileHandle);
	FILE *stream = nullptr;
	int length = 0;

	friend class FileSystemLinux;

public:
	virtual void close() override;

	FileHandleLinux();
	~FileHandleLinux();
};
