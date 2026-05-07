#pragma once

#include "core/io/file_handle.h"
#include "core/typedefs.h"

#include <stdio.h>

class FileHandleLinux : public FileHandle {
	VREGISTER_CLASS(FileHandleLinux, FileHandle);
	FILE *f = nullptr;
	int length = -1;
	String path;

	void _write_generic(const uint8_t *p_buffer, int p_length);

	virtual uint64_t _get_of_size(int p_size) override;
	virtual Error _open_internal(const String &p_path, int p_flags) override;

public:
	virtual void close() override;
	virtual void seek(int p_position) override;

	virtual uint64_t get_position() const override;
	virtual uint64_t get_length() const override;

	virtual Vector<uint8_t> get_buffer(int p_length) override;
	virtual int get_buffer(uint8_t *p_buffer, int p_length) override;

	virtual void store_string(const String &p_string) override;
	virtual void store_buffer(const Vector<uint8_t> &p_buffer) override;

	FileHandleLinux();
	~FileHandleLinux();
};
