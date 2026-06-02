#include "platform/linux/filesystem_linux.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

String FileSystemLinux::get_cwd() const {
	// malloc'd data
	char *data = getcwd(nullptr, 512);
	String ret = data;
	free(data);
	return ret;
}

Error FileSystemLinux::set_cwd(const String &p_path) {
	ERR_FAIL_COND_R(p_path.is_empty(), ERR_INVALID_PATH);

	// Check if the path is a file or a directory
	if (!is_dir(p_path)) {
		return ERR_INVALID_PATH;
	}

	int err = chdir(p_path.get_data());
	if (err == -1) {
		return ERR_DOESNT_EXIST; // is invalid file handle
	}

	return OK;
}

bool FileSystemLinux::is_dir(const String &p_path) const {
	struct stat pstat;
	stat(p_path.get_data(), &pstat);
	return S_ISDIR(pstat.st_mode);
}
