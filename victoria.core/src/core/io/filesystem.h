#pragma once

#include "core/io/file_handle.h"
#include "core/string/vstring.h"
#include "core/typedefs.h"

/**
 * @brief Class that represents a handle to a file in the user's filesystem.
 */
class VCORE_API FileSystem {
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

	/**
	 * @brief Gets the current working directory for the engine, or in other terms it gets the relative path that all
	 * subsequent relative paths are calculated from.
	 * @return The current working directory for the engine.
	 */
	virtual String get_cwd() const = 0;

	/**
	 * @brief Sets the current working directory to a given path. This path may be either relative or absolute, however
	 * the latter is discouraged as it varies between operating systems - paths for engine resources and user data will
	 * be added and available in the future.
	 * @param p_path The path to set the new current working directory to. Must be a directory and not a file.
	 * @return `OK` on success, and `ERR_INVALID_PATH` or `ERR_INVALID_PARAMETER` on failure.
	 */
	virtual Error set_cwd(const String &p_path) = 0;

	/**
	 * @brief Checks to see if the given filepath is a valid directory.
	 * @param p_path The path to check.
	 * @return `true` if valid, `false` if not.
	 */
	virtual bool is_dir(const String &p_path) const = 0;

	FileSystem();
	virtual ~FileSystem();
};
