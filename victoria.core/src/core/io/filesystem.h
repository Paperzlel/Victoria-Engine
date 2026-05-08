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

public:
	/**
	 * @brief Gets the singleton instance for this class. The singleton is allocated at the same time the OS singleton
	 * is, and is held there until destruction of said OS class.
	 */
	static FileSystem *get_singleton();

	template <typename T>
	static void set_default() {
		_create = _create_file_builtin<T>;
	}

	// Enum for all the different file access types one may want to use (duplicate of FileHandleFlags)
	enum FileAccessType {
		FLAG_READ = 1 << 0,								   // Read-only, return NULL if nonexistent
		FLAG_WRITE = 1 << 1,							   // Write-only, create if nonexistent, overwrite contents
		FLAG_READ_WRITE = FLAG_READ | FLAG_WRITE,		   // Read and write, return NULL if nonexistent
		FLAG_WRITE_READ = 1 << 2 | FLAG_READ | FLAG_WRITE, // Read and write, create if nonexistent, overwrite contents
		FLAG_RELATIVE = 1 << 3,							   // Relative file path
	};

	/**
	 * @brief Opens the file specified by the given path and uses a given kind of read/write operation.
	 * @param p_path The path to the given file to load. Can be absolute or relative paths.
	 * @param p_flags The flags one wants to use when opening a file. Operates as a bitmask, so OR the values together.
	 * @param r_error An error flag that is set if the file opening process was unsuccessful.
	 * @return A file handle if successful, and an empty, invalid one if the process failed. Does not print an error
	 * and as such users should throw errors where appropriate.
	 */
	Ref<FileHandle> open_file(const String &p_path, FileAccessType p_flags, Error *r_error);

	FileSystem();
	virtual ~FileSystem();
};
