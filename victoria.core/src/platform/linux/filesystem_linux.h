#pragma once

#include "core/typedefs.h"
#if PLATFORM_LINUX

#	include "core/io/filesystem.h"

class FileSystemLinux : public FileSystem {
public:
	virtual String get_cwd() const override;
	virtual Error set_cwd(const String &p_path) override;

	virtual bool is_dir(const String &p_path) const override;

	FileSystemLinux() {}
	~FileSystemLinux() {}
};

#endif // PLATFORM_LINUX