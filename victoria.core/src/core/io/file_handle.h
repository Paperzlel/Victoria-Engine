#pragma once

#include "core/io/resource.h"
#include "core/typedefs.h"

class VAPI FileHandle : public Resource {
	VREGISTER_CLASS(FileHandle, Resource);
	bool read_only = false;
	int flags = 0;
	String path;

	friend class FileSystem;

public:
	enum FileHandleFlags {
		FLAG_READ = 1 << 0,
		FLAG_WRITE = 1 << 1,
		FLAG_RELATIVE = 1 << 2,
	};

	bool is_read_only() const {
		return read_only;
	}

	void set_flags(int p_flags);
	int get_flags() const;

	/**
	 * @brief Closes the file handle and sets it to nullptr.
	 */
	virtual void close() = 0;

	/**
	 * @brief Obtains the current position of the file handle within the file. File position is read from 0, where 0 is
	 * the space before the first character.
	 */
	virtual uint64_t get_position() const = 0;

	/**
	 * @brief Finds the length (number of characters between 0 and the null terminator) of the currently used file.
	 * Returns to the origin of the file, rather than the previously read file.
	 */
	virtual uint64_t get_length() const = 0;

	virtual bool is_eof() const = 0;

	virtual Vector<uint8_t> get_buffer(int p_length) = 0;
	virtual int get_buffer(uint8_t *p_buffer, int p_length) = 0;

	virtual String get_as_string() = 0;
	virtual String get_line() = 0;
	virtual void store_string(const String &p_string) = 0;
	virtual void store_buffer(const Vector<uint8_t> &p_buffer) = 0;

	FileHandle();
	~FileHandle();
};
