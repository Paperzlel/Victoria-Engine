#include "logger.h"

#include <stdio.h>
#include <string.h>

void Logger::logv(bool is_error, const char *message, va_list arg_ptr) {
    char out_message[MAX_BUF_LEN];
    memset(out_message, 0, MAX_BUF_LEN);

    vsnprintf(out_message, MAX_BUF_LEN, message, arg_ptr);

    fprintf(is_error ? stderr : stdout, "%s\n", out_message);
}

void Logger::log_error(const char *file, const char *function, int line, const char *err_msg, const char *err_details, ErrorType type) {
    static const char *err_strings[4] = {"[ERROR]", "[WARNING]", "[DEBUG]", "[SHADER ERROR]"};
    logf(true, "%s: %s %s\n", err_strings[type], err_msg, err_details);
    logf(true, "    at: %s <%s:%i>\n", file, function, line);
}

void Logger::logf(bool is_error, const char *message, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, message);
    logv(is_error, message, arg_ptr);
    va_end(arg_ptr);
}
