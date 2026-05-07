#pragma once

#include "core/io/filesystem.h"

class FileSystemLinux : public FileSystem {
public:
	virtual Error _open_file(const Ref<FileHandle> &p_handle) override;

	FileSystemLinux() {}
	~FileSystemLinux() {}
};
