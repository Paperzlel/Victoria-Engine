#include "error_macros.h"

#include "core/io/logger.h"

void _err_print_err(const char *file, const char *function, int line, const char *err_msg, const char *msg) {
    //TODO: string comparison functions.
    Logger::get_singleton()->logf(true, "In file %s <function %s, line %i>: \n\tError occured: %s \n\tError message: %s", file, function, line, err_msg, msg);
}

void _err_print_index_err(const char *file, const char *function, int line, int m_index, int m_size, const char *err_msg, const char *msg) {
    //TODO: string comparison functions.
    Logger::get_singleton()->logf(true, "In file %s <function %s, line %i>: \n\tIndex error occured (m_index = %i when m_size = %i) \n\tError message: %s %s", file, \
        function, line, m_index, m_size, err_msg, msg);
}