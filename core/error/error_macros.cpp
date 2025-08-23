#include "error_macros.h"

#include "core/os/os.h"

#include <stdio.h>
#include <string.h>

void _err_print_err(const char *file, const char *function, int line, const char *err_msg, const char *msg, ErrorType p_type) {
    if (OS::get_singleton()) {
        OS::get_singleton()->print_error(file, function, line, err_msg, msg, p_type);
    }
}

void _err_print_index_err(const char *file, const char *function, int line, int m_index, int m_size, const char *err_msg, const char *msg, ErrorType p_type) {
    if (OS::get_singleton()) {
        char buf[Logger::MAX_BUF_LEN];
        memset(buf, 0, Logger::MAX_BUF_LEN);

        sprintf(buf, "m_index was %i when m_size was %i. %s", m_index, m_size, err_msg);

        OS::get_singleton()->print_error(file, function, line, buf, msg, p_type);
    }
}