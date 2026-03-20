#include "core/io/logger.h"

#include "core/os/memory.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief Logs a message to stdout/stderr.
 * @param is_error Checks if the given message should be treated as an error
 * @param message The message to send to stdout/stderr
 * @param arg_list A `va_list` of the non-string items to format into the message
 */
void Logger::logv(bool is_error, const char *message, va_list arg_list) {
    if (is_error) {
        vfprintf(stderr, message, arg_list);
    } else {
        vfprintf(stdout, message, arg_list);
    }
}


/**
 * @brief Sends a formatted error message to be logged to a given destination.
 * @param file The source file it is logging from
 * @param function The function the error comes from
 * @param line The line the error comes from
 * @param message The error message to output to the console
 * @param err_details An (optional) message to inform the user about the specific details of this error
 */
void Logger::log_error(const char *file, const char *function, int line, const char *err_msg, const char *err_details, ErrorType type) {
    static const char *err_strings[4] = {"ERROR", "WARNING", "DEBUG", "SHADER ERROR"};
    logf(true, "%s: %s %s\n", err_strings[type], err_msg, err_details);
    logf(true, "    at: %s <%s:%i>\n", function, file, line);
}

/**
 * @brief Formats an error message and sends it to `logv` to be logged in a given destination.
 * @param is_error Checkes if the given message should be formatted as an error, which will be formatted differently depending on the platform used
 * @param message The message to send to the console
 * @param ... Variatic arguments to format with a va_list into the message
 */
void Logger::logf(bool is_error, const char *message, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, message);

    logv(is_error, message, arg_ptr);

    va_end(arg_ptr);
}
