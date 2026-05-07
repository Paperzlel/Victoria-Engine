#pragma once

#include "core/object/ref_counted.h"
#include "core/typedefs.h"

class VAPI FileHandle : public RefCounted {
	VREGISTER_CLASS(FileHandle, RefCounted);

	int flags = 0; // What flags are set for the file.
	friend class FileSystem;

	/**
	 * @brief Gets the next value of size N. Should increment the stream by N bytes while doing so.
	 * @param p_size The size of the item in bytes. This value cannot be less than 8 and must be a power of 2
	 * @return The value encoded as a 64-bit integer that can be casted down.
	 */
	virtual uint64_t _get_of_size(int p_size = 1) = 0;

	/**
	 * @brief The OS-specific implementation of opening a file. Kept internal for API reasons.
	 * @param p_path The path to the file to open. The path can be either a valid UNIX file or a Windows file handle,
	 * the implementation will correct the path itself.
	 * @param p_flags Any flags that will modify how a file is used. Should never be 0.
	 * @return `OK`, on success, and an `ERR_FILE_*` error on failure.
	 */
	virtual Error _open_internal(const String &p_path, int p_flags) = 0;

public:
	// Flags that represent the different ways a file handler can be opened.
	enum FileHandleFlags {
		FLAG_READ = 1 << 0,								   // Read-only, return NULL if nonexistent
		FLAG_WRITE = 1 << 1,							   // Write-only, create if nonexistent, overwrite contents
		FLAG_READ_WRITE = FLAG_READ | FLAG_WRITE,		   // Read and write, return NULL if nonexistent
		FLAG_WRITE_READ = 1 << 2 | FLAG_READ | FLAG_WRITE, // Read and write, create if nonexistent, overwrite contents
		FLAG_RELATIVE = 1 << 3,							   // Relative file path
	};

	/**
	 * @brief Set the flags for the file. Do not use outside of internal creation, instead close and re-open the file.
	 * @param p_flags The flags to set for the given file.
	 */
	void set_flags(int p_flags);

	/**
	 * @brief Get the flags set for this given file handle.
	 * @return The flags for this given file handle.
	 */
	int get_flags() const;

	/**
	 * @brief Checks to see if the given file handle is at the end of the file stream and must be sought back to
	 * another point.
	 * @return `true` if the file is at the end-of-file, `false` if not and more data can be read.
	 */
	bool is_eof() const;

	/**
	 * @brief Gets another 8-bit unsigned integer from the given handle. This value may be casted however the user sees
	 * fit. Moves the file pointer along by 1 byte.
	 * @return An 8-bit unsigned integer.
	 */
	uint8_t get_8();

	/**
	 * @brief Gets another 16-bit unsigned integer from the given handle. This value may be casted however the user
	 * sees fit. Moves the file pointer along by 2 bytes.
	 * @return A 16-bit unsigned integer.
	 */
	uint16_t get_16();

	/**
	 * @brief Gets another 32-bit unsigned integer from the given handle. This value may be casted however the user
	 * sees fit. Moves the file pointer along by 4 bytes.
	 * @return A 32-bit unsigned integer.
	 */
	uint32_t get_32();

	/**
	 * @brief Gets another 64-bit unsigned integer from the given handle. This value may be casted however the user
	 * sees fit. Moves the file pointer along by 8 bytes.
	 * @return An unsigned 64-bit integer.
	 */
	uint64_t get_64();

	/**
	 * @brief Gets a line from the given file. Assumes that the line is a series of UTF-8 encoded characters than ends
	 * with a newline (`\n`) character, a NUL terminator, or the EOF indicator.
	 * @return A copy of the data from the file as a string.
	 */
	String get_line();

	/**
	 * @brief Gets the entire contents of the file as a singular string. Assumes that the file is a series of UTF-8
	 * encoded characters than ends with an EOF indicator. Avoid using this method on large files, as it is likely to
	 * cause RAM errors if too many are loaded at once.
	 * @return The entire contents of the file.
	 */
	String get_as_string();

	/**
	 * @brief Closes the file handle and sets it to null. The file handle is now inactive and should be left to go out
	 * of scope.
	 */
	virtual void close() = 0;

	/**
	 * @brief Seeks the file path to the given location. The location is in bytes, and must be a valid integer between
	 * 0 and the size of the file in bytes.
	 * @param p_position The position to move the file pointer to.
	 */
	virtual void seek(int p_position) = 0;

	/**
	 * @brief Obtains the current position of the file handle within the file. File position is read from 0, where 0 is
	 * the space before the first character.
	 */
	virtual uint64_t get_position() const = 0;

	/**
	 * @brief Finds the length (number of characters between 0 and the null terminator) of the currently used file.
	 * Does not affect the position of the file handler.
	 */
	virtual uint64_t get_length() const = 0;

	/**
	 * @brief Gets a buffer representing a region of the file from the current file pointer position to the position +
	 * length. Moves the file pointer to the new position in the process.
	 * @param p_length The number of bytes to read from the file.
	 * @return A Vector containing the bytes read from the file.
	 */
	virtual Vector<uint8_t> get_buffer(int p_length) = 0;

	/**
	 * @brief Reads the data from the file pointer into a given buffer. The data is read from the file pointer position
	 * to the file pointer position + length. Moves the file pointer to the new offset in the process.
	 * @param p_buffer A pointer to a valid region of memory with size length. If the buffer is null, we return 0.
	 * @param p_length The number of bytes to read into the buffer.
	 * @return The number of bytes actually read. If this value is 0, an error may have occured or the file is at the
	 * EOF. If this value is less than the requested length, an error in reading may have occured, or the file reach
	 * the EOF before it expected to do so.
	 */
	virtual int get_buffer(uint8_t *p_buffer, int p_length) = 0;

	/**
	 * @brief Stores a given string inside the file pointer, starting from the current file pointer position. Does not
	 * include the null terminator, as it expects the file to be opened as a text file in this scenario.
	 * @param p_string The string to insert into the buffer. Must be a valid string of at least 1 byte and be
	 * null-terminated.
	 */
	virtual void store_string(const String &p_string) = 0;

	/**
	 * @brief Stores a buffer in the given file pointer, starting from the current file pointer position. Acts as a
	 * continuous span of memory and does not respect any null-terminating procedures, use `store_string()` for that
	 * purpose. If one wishes to append non-byte data, they must first convert their vector to a byte array and pass
	 * that in instead.
	 * @param p_buffer The buffer in question to store in the file. Must be a valid vector of at least 1 byte.
	 */
	virtual void store_buffer(const Vector<uint8_t> &p_buffer) = 0;

	FileHandle();
	~FileHandle();
};
