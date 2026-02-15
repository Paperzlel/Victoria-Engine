#pragma once

#include <core/typedefs.h>
#include <core/string/vstring.h>
#include <core/error/error_macros.h>

#include "resource.h"

class FileHandler;

/**
 * @brief Class that represents a handle to a file in the user's filesystem.
 */
class VAPI FileSystem : public Resource {
    VREGISTER_CLASS(FileSystem, Resource)
protected:

    FileHandler *handle = nullptr;
    bool is_read_only = false;
public:

    // Enum for all the different file access types one may want to use
    enum FileAccessType {
        FILE_ACCESS_READ,
        FILE_ACCESS_WRITE,
        FILE_ACCESS_MAX
    };

    static Ref<FileSystem> open(const String &p_path, FileAccessType access_type);
    void close();

    bool is_valid_file() const;

    u64 get_position() const;
    u64 get_length() const;
    bool is_eof() const;

    Vector<u8> get_buffer(int p_length);
    int get_buffer(u8 *p_buf, int p_length);

    String get_contents_as_string();
    String get_line();
    void store_string(const String &p_string);

    static String fix_path(const String &p_path);

    FileSystem();
    ~FileSystem();
};
