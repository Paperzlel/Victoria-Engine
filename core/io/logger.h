#pragma once

#include <stdarg.h>

/**
 * @brief Base logging class. By default, it writes messages to stdout but can be configured to write messages elsewhere.
 */
class Logger {
public:
    static const int MAX_BUF_LEN = 32000;

    // The different types of errors our code can generate. 
    enum ErrorType {
        ERROR_ERR,
        ERROR_WARNING,
        ERROR_DEBUG,
        ERROR_SHADER,
    };
    
    /**
     * @brief Virtual function that acts to log a message wherever it points to. Different extensions of this class can log to
     * different places. For API use cases, please use `OS::print()` if one needs message in the console.
     * @param is_error Whether to log the text as an error message or not. By default, logs an error message to stderr if true,
     * and stdout if false.
     * @param p_message The message to format and then print.
     * @param arg_ptr Variadic arguments in which to format to the output string
     */
    virtual void logv(bool is_error, const char *message, va_list arg_ptr);

    /**
     * @brief Logs an error message to a given location. Error messages contain the file, function and line of the error, as well
     * as the error message and details. Error types are also forwarded to allow the implementation of handling different error
     * types.
     * @param file The file that the error occured in. Obtained through the macro `__FILE__` on most compilers.
     * @param function The function that the error occured in. Obtained via our `FUNCTION_STR` macro in `core/error/error_macros.h`.
     * @param line The line number the error occured in. Obtained via the `__LINE__` on most compilers.
     * @param err_msg The error message to print into the console. Usually a generic error that can occur.
     * @param err_details The details about the error that occured.
     * @param type The type of error that occured, as defined by the `ErrorType` enum.
     */
    virtual void log_error(const char *file, const char *function, int line, const char *err_msg, const char *err_details, ErrorType type);
    
    /**
     * @brief Variadic logging function. Takes the message and its arguments and fills them into a `va_list`, then sends it to the
     * current implementation of `logv`.
     * @param message The message to send, incoluding the format specifiers.
     * @param ... Variadic arguments to pass to `logv`.
     */
    void logf(bool is_error, const char *message, ...);

    virtual ~Logger() {}
};
