#include "core/io/filesystem.h"

FileSystem *FileSystem::singleton = nullptr;

FileSystem *FileSystem::get_singleton() {
	return singleton;
}

Ref<FileHandle> FileSystem::open_file(const String &p_path, FileAccessType p_flags, Error *r_error) {
	// Create a new handle
	Ref<FileHandle> s = _create();

	if (s.is_null()) {
		return Ref<FileHandle>();
	}

	s->set_flags(p_flags);
	Error err = s->_open_internal(p_path, p_flags);
	if (r_error) {
		*r_error = err;
		if (err) {
			return Ref<FileHandle>();
		}
	}

	return s;
}

/**
 * @brief Class constructor for a FileSystem object.
 */
FileSystem::FileSystem() {
	singleton = this;
}

/**
 * @brief Class destructor for the FileSystem.
 */
FileSystem::~FileSystem() {
	singleton = nullptr;
}