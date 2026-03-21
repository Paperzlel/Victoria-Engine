#include "core/io/filesystem.h"

#include <stdio.h>

/**
 * @brief Internal class that holds all of the file data one needs.
 */
class FileHandler {
public:
	FILE *f = nullptr;
	int pos = 0;
};

/**
 * @brief Opens the file specified by the given path and uses a given kind of read/write operation. Currently no
 * support for reading AND writing.
 * @param p_path The path to the given file to load. Can be absolute, but relative paths are relative to the place of
 * the `victoria` binary on your PC.
 * @param access_type The method one wants to open the file with, Currently is reading or writing, but not both.
 */
Ref<FileSystem> FileSystem::open(const String &p_path, FileAccessType access_type) {
	// Create a new handle
	Ref<FileSystem> s;
	s.instantiate();

	switch (access_type) {
		case FILE_ACCESS_READ: {
			s->handle->f = fopen(p_path, "rb");
			// By default, assume you want to lock it to read-only
			s->is_read_only = true;
		} break;
		case FILE_ACCESS_WRITE: {
			s->handle->f = fopen(p_path, "w");
		} break;
		// Assume by default the user wants to read only
		default: {
			s->handle->f = fopen(p_path, "r");
			s->is_read_only = true;
		}
	}

	ERR_COND_NULL_MSG_R(s->handle->f, "The path entered was incorrect.", Ref<Resource>());
	return s;
}

/**
 * @brief Closes the file handle and sets it to nullptr.
 */
void FileSystem::close() {
	if (handle->f) {
		fclose(handle->f);
		handle->f = nullptr;
	}
}

/**
 * @brief Checks to see if the current file contains a valid file handler.
 * @returns `true` if the file does exist and can be opened, and `false` if not.
 */
bool FileSystem::is_valid_file() const {
	return handle->f != nullptr;
}

/**
 * @brief Obtains the current position of the file handle within the file. File position is read from 0, where 0 is the
 * space before the first character.
 */
u64 FileSystem::get_position() const {
	return ftell(handle->f);
}

/**
 * @brief Finds the length (number of characters between 0 and the null terminator) of the currently used file. Returns
 * to the origin of the file, rather than the previously read file.
 */
u64 FileSystem::get_length() const {
	if (!handle->f) {
		return 0;
	}

	u64 pos = get_position();
	fseek(handle->f, 0, SEEK_END);
	u64 size = get_position();
	fseek(handle->f, pos, SEEK_SET);

	return size;
}

/**
 * @brief Checks to see if the current file stream is being read from the end of the file.
 */
bool FileSystem::is_eof() const {
	return get_position() >= get_length();
}

Vector<u8> FileSystem::get_buffer(int p_length) {
	Vector<u8> ret;
	ret.resize(p_length);

	u8 *ptr = ret.ptrw();

	int len = get_buffer(ptr, p_length);
	if (len < p_length) {
		ret.resize(len);
	}
	return ret;
}

int FileSystem::get_buffer(u8 *p_buf, int p_length) {
	return fread(p_buf, 1, p_length, handle->f);
}

/**
 * @brief Reads all of the text within a file as a string and returns that to the user, as well as returning the file
 * position back to the start.
 */
String FileSystem::get_contents_as_string() {
	ERR_COND_NULL_R(handle->f, String());
	u64 file_size = get_length();

	String p_string;
	p_string.resize(file_size + 1);
	fread(p_string.ptrw(), 1, file_size, handle->f);
	fseek(handle->f, 0, SEEK_SET);
	p_string[file_size] = 0;

	return p_string;
}

/**
 * @brief Gets the next line of the current file. A line is specified as the point between the start of the file and
 * its first newline character, between two newline characters, or between one newline character and the end of the
 * file.
 * @returns The next line of the currently active file
 */
String FileSystem::get_line() {
	ERR_COND_NULL_MSG_R(handle->f, "Couldn't obtain a line count as the file was not opened.", String());

	fseek(handle->f, handle->pos, SEEK_SET);
	int offset = 0;
	int c = 0;
	while (c != EOF) {
		if (c == (int)'\n') {
			break;
		}
		c = fgetc(handle->f);
		// Only increment if CR character is not found
		if (c != (int)'\r') {
			offset++;
		}
	}

	if (offset == 1) {
		handle->pos += 1;
		return String();
	}

	String ret;
	ret.resize(offset); // offset naturally adds 1 to itself, so string resizing doesn't need to account for the null
						// terminator
	fseek(handle->f, handle->pos, SEEK_SET);
	fread(ret.ptrw(), 1, offset, handle->f);
	ret[offset - 1] = 0;

	handle->pos += offset;
	return ret;
}

/**
 * @brief WARNING: NOT CURRENTLY IMPLEMENTED. DO NOT USE.
 */
void FileSystem::store_string(const String &p_string) {}

/**
 * @brief WARNING: NOT CURRENTLY IMPLEMENTED. DO NOT USE.
 */
String FileSystem::fix_path(const String &p_path) {
	return String(); // TODO: Fix
}

/**
 * @brief Class constructor for a FileSystem object.
 */
FileSystem::FileSystem() {
	handle = vnew(FileHandler);
}

/**
 * @brief Class destructor for a FileSystem object. Closes any open files and frees the current handle.
 */
FileSystem::~FileSystem() {
	close();
	vdelete(handle);
	handle = nullptr;
}