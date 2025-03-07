#pragma once

#include <stdarg.h>

class Logger {
public:
    static const int MAX_BUF_LEN = 32000;

    enum ErrorType {
        ERROR_ERR,
        ERROR_WARNING,
        ERROR_DEBUG,
        ERROR_SHADER,
    };
    
    virtual void logv(bool is_error, const char *message, va_list arg_ptr);
    virtual void log_error(const char *file, const char *function, int line, const char *err_msg, const char *err_details, ErrorType type);
    
    void logf(bool is_error, const char *message, ...);

    virtual ~Logger() {}
};
