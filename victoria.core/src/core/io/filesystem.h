#pragma once

#include "core/io/file_handle.h"
#include "core/string/vstring.h"
#include "core/typedefs.h"

/**
 * @brief Class that represents a handle to a file in the user's filesystem.
 */
class VAPI FileSystem {
	static FileSystem *singleton;

	typedef Ref<FileHandle> (*FileHandleCreateFunc)();
	static inline FileHandleCreateFunc _create = nullptr;

	template <typename T>
	static Ref<FileHandle> _create_file_builtin() {
		return vnew(T);
	}

	virtual Error _open_file(const Ref<FileHandle> &p_handle) = 0;

public:
	static void get_singleton();

	template <typename T>
	static void set_default() {
		_create = _create_file_builtin<T>;
	}

	// Enum for all the different file access types one may want to use
	enum FileAccessType {
		FILE_ACCESS_READ = 1 << 0,
		FILE_ACCESS_WRITE = 1 << 1,
	};

	/**
	 * @brief Opens the file specified by the given path and uses a given kind of read/write operation. Currently no
	 * support for reading AND writing.
	 * @param p_path The path to the given file to load. Can be absolute, but relative paths are relative to the place
	 * of the `victoria` binary on your PC.
	 * @param access_type The method one wants to open the file with, Currently is reading or writing, but not both.
	 */
	Ref<FileHandle> open_file(const String &p_path, FileAccessType access_type);

	FileSystem();
	virtual ~FileSystem();
};
