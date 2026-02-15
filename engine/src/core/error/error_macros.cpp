#include <core/error/error_macros.h>

#include <core/os/os.h>
#include <core/string/vstring.h>

#include <stdio.h>
#include <string.h>

void _err_print_err(const char *file, const char *function, int line, const char *err_msg, const char *msg, ErrorType type) {
    if (OS::get_singleton()) {
        OS::get_singleton()->print_error(file, function, line, err_msg, msg, type);
    }
}

void _err_print_index_err(const char *file, const char *function, int line, int m_index, int m_size, const char *err_msg, const char *msg, ErrorType type) {
    if (OS::get_singleton()) {
        String buf = vformat("m_index was %i when m_size was %i. %s", m_index, m_size, err_msg);

        OS::get_singleton()->print_error(file, function, line, buf.get_data(), msg, type);
    }
}

const char *get_error_message(Error p_id) {
    return error_messages[p_id];
}