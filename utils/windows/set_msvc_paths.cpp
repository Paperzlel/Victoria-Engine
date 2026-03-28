#ifdef _MSC_VER
// Silences clangd errors about redefinition.
#	ifndef _CRT_SECURE_NO_WARNINGS
#		define _CRT_SECURE_NO_WARNINGS
#	endif
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <winreg.h>

#	include <stdio.h>
#	include <string.h>
#	include <stdlib.h>

// I prefer NOT using the C++ libraries, but since it's a massive utility bonus here, I'll give it a pass.
#	include <string>

#	if _MSC_VER < 1800
#		error "Visual Studio versions less than 2015 have a different PATH layout. This tool will not work."
#	endif

/**
 * Sets the environment variables for Windows prior to actually being able to set them.
 * This file is built as a utility batch file from "misc/windows/build_msvc_paths" to
 * compensate.
 * We DO support the existence of x86/ARM, but the engine isn't yet designed with support for it. Sorry.
 */

#	define BUF_MAX_LEN 32767

static LPCSTR get_env(LPCSTR env) {
	char buf[BUF_MAX_LEN];
	memset(buf, 0, BUF_MAX_LEN);
	DWORD len = GetEnvironmentVariableA(env, buf, BUF_MAX_LEN);
	if (!len) {
		return nullptr;
	}
	buf[len] = 0;
	return _strdup(buf);
}

static DWORD get_reg_data(HKEY key, LPCSTR value, void **data, LPDWORD r_type) {
	// Alloc an array for all buffer data
	DWORD size = 0;
	DWORD type = 0;
	LONG result = RegGetValueA(key, "", value, RRF_RT_ANY, &type, nullptr, &size);
	if (result != ERROR_SUCCESS) {
		// Silent error for when we need to handle a "key-not-found" error.
		if (result == ERROR_FILE_NOT_FOUND) {
			*r_type = REG_SZ;
			return 0;
		}
		printf("Unable to get the registry key value (error 0x%lx).\n", result);
		return 0;
	}

	void *rdata = malloc((size_t)(size));
	memset(rdata, 0, size);
	result = RegGetValueA(key, "", value, RRF_RT_ANY, &type, rdata, &size);
	if (result != ERROR_SUCCESS) {
		printf("Failed to get registry key data (error 0x%lx)\n", result);
		return 0;
	}

	*data = rdata;
	*r_type = type;
	return size;
}

static bool contains_key(char *env, const std::string &key) {
	while (env) {
		if (memcmp(env, key.c_str(), key.length()) == 0) {
			return true;
		}

		env = strstr(env, ";");
		if (!env) {
			break;
		}
		env++;
	}

	return false;
}

int main(void) {
	/**
	 * Args available:
	 * --help: print message
	 * --version: version of the command
	 */
	// Get the VC tool directory
	LPCSTR tools_dir = get_env("VcToolsInstallDir");
	LPCSTR arch = get_env("VSCMD_ARG_TGT_ARCH");
	LPCSTR windows_sdk = get_env("WindowsSdkDir");
	LPCSTR windows_sdk_version = get_env("WindowsSDKVersion");
	LPCSTR llvm_x64 = get_env("llvmX64");

	if (!tools_dir || !arch || !windows_sdk || !windows_sdk_version) {
		printf("You appear to not have the Visual Studio tools or the Windows SDK installed, or have not called this "
			   "executable outside of a valid Visual Studio shell. Unable to get the proper environment variables to "
			   "setup compilation for MSVC.\n");
		return 1;
	}

	bool add_llvm_installations = false;
	if (llvm_x64) {
		printf("Detected LLVM installation, would you like to add LLVM to the system PATH? (y/n)");
		char input[BUFSIZ] = {};
		scanf("%s", input);
		switch (input[0]) {
			case 'y':
			case 'Y':
			case '\0':
			case '\n':
			case '\r':
				add_llvm_installations = true;
				break;
			case 'n':
			case 'N':
			default:
				add_llvm_installations = false;
				break;
		}
	}

	// Get strings in the form "<VC dir>\bin\Host<arch>\<arch>"
	std::string bin;
	bin = tools_dir;
	bin += "bin\\Host";
	bin += arch;
	bin += "\\";
	bin += arch;
	bin += ";";
	if (add_llvm_installations) {
		// Add x64 first to prevent confusion
		if (llvm_x64) {
			bin += llvm_x64;
			bin += ";";
		}
	}

	std::string include;
	// MSVC tools dir
	include = tools_dir;
	include += "include";
	include += ";";
	// Windows SDK UCRT includes
	include += windows_sdk;
	include += "Include\\";
	include += windows_sdk_version;
	include += "ucrt;";
	// Windows SDK UM includes
	include += windows_sdk;
	include += "Include\\";
	include += windows_sdk_version;
	include += "um;";
	// Windows SDK shared includes
	include += windows_sdk;
	include += "Include\\";
	include += windows_sdk_version;
	include += "shared;";

	std::string libs;
	// MSVC libs
	libs = tools_dir;
	libs += "lib\\";
	libs += arch;
	libs += ";";
	// Windows SDK UCRT libs
	libs += windows_sdk;
	libs += "Lib\\";
	libs += windows_sdk_version;
	libs += "ucrt\\";
	libs += arch;
	libs += ";";
	// Windows SDK UM libs
	libs += windows_sdk;
	libs += "Lib\\";
	libs += windows_sdk_version;
	libs += "um\\";
	libs += arch;
	libs += ";";

	HKEY registry_key;
	LPCTSTR subkey = TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
	DWORD disposition;
	LONG result = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
								  subkey,
								  0,
								  nullptr,
								  0,
								  KEY_ALL_ACCESS,
								  nullptr,
								  &registry_key,
								  &disposition);
	if (result != ERROR_SUCCESS) {
		if (result == ERROR_ACCESS_DENIED) {
			printf("Unable to open the registry key: access is denied (error 0x%lx). Please run this executable with "
				   "administrator privileges.",
				   result);
			return 2;
		}
		printf("Unable to open the registry key for setting the windows environment (error 0x%lx).\n", result);
		return 2;
	}

	if (disposition == REG_CREATED_NEW_KEY) {
		printf("Created a new key - apparently your environment doesn\'t exist!\n");
		return 100;
	}

	// Alloc an array for all buffer data
	char *path = nullptr;
	DWORD type = 0;
	DWORD size = get_reg_data(registry_key, "Path", (void **)&path, &type);
	if (!size && !type) {
		printf("Unable to load the system PATH from the registry.\n");
		return 2;
	}

	if (type != REG_SZ) {
		printf("Data must be in a single-string format.\n");
		return 3;
	}

	bool needs_path_key = !contains_key(path, bin);
	if (needs_path_key) {
		if (!path) {
			path = (char *)malloc(bin.size());
			memcpy(path, bin.c_str(), bin.length());
			path[include.size()] = 0;
		} else {
			path = (char *)realloc(path, size + bin.size());
			path[size] = 0;
			path = strcat(path, bin.c_str());
		}

		result = RegSetValueExA(registry_key, "Path", 0, type, (const BYTE *)path, (DWORD)strlen(path) + 1);
		if (result != ERROR_SUCCESS) {
			printf("Unable to set the registry key to the given value (error 0x%lx)\n", result);
		}
		printf("Set system PATH successfully.\n");
	} else {
		printf("Skipping path key as it is not required.\n");
	}

	char *includepath = nullptr;
	type = 0;
	size = get_reg_data(registry_key, "INCLUDE", (void **)&includepath, &type);
	if (!size && !type) {
		printf("Unable to load the INCLUDE environment variable from the registry.\n");
		return 2;
	}

	if (type != REG_SZ) {
		printf("Expected data in a single-string format.\n");
		return 3;
	}

	bool needs_include_key = !contains_key(includepath, include);
	if (needs_include_key) {
		if (!includepath) {
			includepath = (char *)malloc(include.size());
			memcpy(includepath, include.c_str(), include.length());
			includepath[include.size()] = 0;
		} else {
			includepath = (char *)realloc(includepath, size + include.size());
			includepath[size] = 0;
			includepath = strcat(includepath, include.c_str());
		}

		result = RegSetValueExA(registry_key,
								"INCLUDE",
								0,
								type,
								(const BYTE *)includepath,
								(DWORD)strlen(includepath) + 1);
		if (result != ERROR_SUCCESS) {
			printf("Unable to set the registry key to the given value (error 0x%lx)\n", result);
		}
		printf("Set system INCLUDE successfully.\n");
	} else {
		printf("Skipping INCLUDE key as it is not required.\n");
	}

	char *libbpath = nullptr;
	type = 0;
	size = get_reg_data(registry_key, "LIB", (void **)&libbpath, &type);
	if (!size && !type) {
		printf("Unable to load the LIB environment variable from the registry.\n");
		return 2;
	}

	if (type != REG_SZ) {
		printf("Expected data in a single-string format.\n");
		return 3;
	}

	bool needs_lib_key = !contains_key(libbpath, libs);
	if (needs_lib_key) {
		if (!libbpath) {
			libbpath = (char *)malloc(libs.size());
			memcpy(libbpath, libs.c_str(), libs.length());
			libbpath[include.size()] = 0;
		} else {
			libbpath = (char *)realloc(libbpath, size + libs.size());
			libbpath[size] = 0;
			libbpath = strcat(libbpath, libs.c_str());
		}

		result = RegSetValueExA(registry_key, "LIB", 0, type, (const BYTE *)libbpath, (DWORD)strlen(libbpath) + 1);
		if (result != ERROR_SUCCESS) {
			printf("Unable to set the registry key to the given value (error 0x%lx)\n", result);
		}
		printf("Set system LIB successfully.\n");
	} else {
		printf("Skipping LIB key as it is not required.\n");
	}

	result = RegCloseKey(registry_key);
	if (result != ERROR_SUCCESS) {
		printf("Failed to clean up registry key (error 0x%lx)", result);
	}

	// Notify system of changes to the registry.
	const char *envstr = "Environment";
	SendNotifyMessageA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)(&envstr[0]));

	// Free used variables
	free((void *)libbpath);
	free((void *)includepath);
	free((void *)path);

	libs.clear();
	include.clear();
	bin.clear();

	free((void *)windows_sdk_version);
	free((void *)windows_sdk);
	free((void *)arch);
	free((void *)tools_dir);

	printf("All done.\n");
	return 0;
}

#endif