#include "core/io/filesystem.h"

Ref<FileHandle> FileSystem::open_file(const String &p_path, FileAccessType access_type) {
	// Create a new handle
	Ref<FileHandle> s = _create();

	int flags = 0;
	if (access_type & FILE_ACCESS_READ) {
		flags |= FileHandle::FLAG_READ;
	}

	if (access_type & FILE_ACCESS_WRITE) {
		flags |= FileHandle::FLAG_WRITE;
	}

	if (!p_path.begins_with("C:\\") || !p_path.begins_with("/")) {
		flags |= FileHandle::FLAG_RELATIVE;
	}

	s->set_flags(flags);
	s->set_path(p_path);
	_open_file(s);
	return s;
}

// uint64_t FileSystem::get_length() const {
// 	if (!handle->f) {
// 		return 0;
// 	}

// 	uint64_t pos = get_position();
// 	fseek(handle->f, 0, SEEK_END);
// 	uint64_t size = get_position();
// 	fseek(handle->f, pos, SEEK_SET);

// 	return size;
// }

// /**
//  * @brief Checks to see if the current file stream is being read from the end of the file.
//  */
// bool FileSystem::is_eof() const {
// 	return get_position() >= get_length();
// }

// Vector<uint8_t> FileSystem::get_buffer(int p_length) {
// 	Vector<uint8_t> ret;
// 	ret.resize(p_length);

// 	uint8_t *ptr = ret.ptrw();

// 	int len = get_buffer(ptr, p_length);
// 	if (len < p_length) {
// 		ret.resize(len);
// 	}
// 	return ret;
// }

// int FileSystem::get_buffer(uint8_t *p_buf, int p_length) {
// 	return fread(p_buf, 1, p_length, handle->f);
// }

// /**
//  * @brief Reads all of the text within a file as a string and returns that to the user, as well as returning the
//  file
//  * position back to the start.
//  */
// String FileSystem::get_contents_as_string() {
// 	ERR_COND_NULL_R(handle->f, String());
// 	uint64_t file_size = get_length();

// 	String p_string;
// 	p_string.resize(file_size + 1);
// 	fread(p_string.ptrw(), 1, file_size, handle->f);
// 	fseek(handle->f, 0, SEEK_SET);
// 	p_string[file_size] = 0;

// 	return p_string;
// }

// /**
//  * @brief Gets the next line of the current file. A line is specified as the point between the start of the file and
//  * its first newline character, between two newline characters, or between one newline character and the end of the
//  * file.
//  * @returns The next line of the currently active file
//  */
// String FileSystem::get_line() {
// 	ERR_COND_NULL_MSG_R(handle->f, "Couldn't obtain a line count as the file was not opened.", String());

// 	fseek(handle->f, handle->pos, SEEK_SET);
// 	int offset = 0;
// 	int c = 0;
// 	while (c != EOF) {
// 		if (c == (int)'\n') {
// 			break;
// 		}
// 		c = fgetc(handle->f);
// 		// Only increment if CR character is not found
// 		if (c != (int)'\r') {
// 			offset++;
// 		}
// 	}

// 	if (offset == 1) {
// 		handle->pos += 1;
// 		return String();
// 	}

// 	String ret;
// 	ret.resize(offset); // offset naturally adds 1 to itself, so string resizing doesn't need to account for the null
// 						// terminator
// 	fseek(handle->f, handle->pos, SEEK_SET);
// 	fread(ret.ptrw(), 1, offset, handle->f);
// 	ret[offset - 1] = 0;

// 	handle->pos += offset;
// 	return ret;
// }

/**
 * @brief WARNING: NOT CURRENTLY IMPLEMENTED. DO NOT USE.
 */
// void FileSystem::store_string(const String &p_string) {
// 	int written = fputs(p_string.get_data(), handle->f);
// 	ERR_FAIL_COND_MSG(written != p_string.length(), "Unable to write full string into file.");
// }

/**
 * @brief WARNING: NOT CURRENTLY IMPLEMENTED. DO NOT USE.
 */
// String FileSystem::fix_path(const String &p_path) {
// 	if (p_path.find("\\")) {
// 		String ret = p_path;
// 		ret.replace('\\', '/');
// 		return ret;
// 	}
// 	return p_path;
// }

/**
 * @brief Class constructor for a FileSystem object.
 */
FileSystem::FileSystem() {
	singleton = this;
}

/**
 * @brief Class destructor for a FileSystem object. Closes any open files and frees the current handle.
 */
FileSystem::~FileSystem() {
	if (singleton) {
		singleton = nullptr;
	}
}