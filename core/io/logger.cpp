#include "logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

Logger *Logger::singleton = nullptr;

Logger *Logger::get_singleton() {
    return singleton;
}

void Logger::logv(bool is_error, const char *message, __builtin_va_list va_list) {
    char out_message[MAX_BUF_LEN];
    memset(out_message, 0, MAX_BUF_LEN);

    vsnprintf(out_message, MAX_BUF_LEN, message, va_list);

    printf("%s\n", out_message);
}

void Logger::logf(bool is_error, const char *message, ...) {
    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    logv(is_error, message, arg_ptr);
    va_end(arg_ptr);
}

void Logger::log_debug_v(const char *file, const char *function, int line, const char *message, ...) {
    char format_debug_buf[MAX_BUF_LEN];
    memset(format_debug_buf, 0, MAX_BUF_LEN);

    __builtin_va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(format_debug_buf, MAX_BUF_LEN, message, arg_ptr);
    va_end(arg_ptr);

    logf(false, "[DEBUG]: In file %s <function %s, line %i>: \n\t%s", file, function, line, format_debug_buf);
}

Logger::Logger() {
    singleton = this;
}
