#pragma once

#include <stdarg.h>

/**
 * @brief Base logging class. By default cannot log anywhere due to `logv` being a virtual method.
 */
class Logger {
public:

    // Constant for the maximum number of characters permitted in a single message. This cap is high enough to where it is unlikely any message will not fit into this buffer, but still small enough to fit onto the stack.
    static const int MAX_BUF_LEN = 32000;

    enum ErrorType {
        ERROR_ERR,
        ERROR_WARNING,
        ERROR_DEBUG,
        ERROR_SHADER,
    };

    virtual void logv(bool is_error, const char *message, va_list arg_list);
    virtual void log_error(const char *file, const char *function, int line, const char *err_msg, const char *err_details, ErrorType type);

    void logf(bool is_error, const char *message, ...);

    virtual ~Logger() {}
};
