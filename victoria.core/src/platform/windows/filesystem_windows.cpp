#include "platform/windows/filesystem_windows.h"
#if PLATFORM_WINDOWS

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	define MAX_BUF_LEN 512

String FileSystemWindows::get_cwd() const {
	char buf[MAX_BUF_LEN];
	Memory::vmemset(buf, 0, MAX_BUF_LEN);
	int len = GetCurrentDirectoryA(MAX_BUF_LEN, buf);
	if (len == 0) {
		return String();
	}

	String ret;
	ret.resize(len + 1);
	Memory::vmemcpy(ret.ptrw(), buf, len);
	ret[len] = 0;
	return ret;
}

Error FileSystemWindows::set_cwd(const String &p_path) {
	String in = p_path;
	in.replace('/', '\\');
	if (!in.ends_with("\\")) {
		in.append("\\");
	}

	if (!is_dir(in)) {
		return ERR_INVALID_PATH;
	}

	if (!SetCurrentDirectoryA(in.ptr())) {
		return ERR_DOESNT_EXIST;
	}

	return OK;
}

bool FileSystemWindows::is_dir(const String &p_path) const {
	DWORD bits = GetFileAttributesA(p_path.ptr());
	if (bits == INVALID_FILE_ATTRIBUTES) {
		return false;
	}

	return bits & FILE_ATTRIBUTE_DIRECTORY;
}

#endif // PLATFORM_WINDOWS