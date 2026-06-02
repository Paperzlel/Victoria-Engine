#pragma once

#include "core/io/filesystem.h"

class FileSystemLinux : public FileSystem {
public:
	virtual String get_cwd() const override;
	virtual Error set_cwd(const String &p_path) override;

	virtual bool is_dir(const String &p_path) const override;

	FileSystemLinux() {}
	~FileSystemLinux() {}
};
