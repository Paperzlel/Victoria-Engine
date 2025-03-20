#include "logger_linux.h"
#if PLATFORM_LINUX

#include <stdio.h>
#include <string.h>

/**
 * @brief Prints a message into the console. This version formats the message to appear with some colour to indicate if it is an error or not.
 * @param is_error Whether the message being passed through should be printed like an error or not
 * @param message The message to output into the console
 * @param arg_list The arguments one wants to formation into the message
 */
void LinuxLogger::logv(bool is_error, const char *message, va_list arg_list) {
    char buf[MAX_BUF_LEN];
    memset(buf, 0, MAX_BUF_LEN);

    vsnprintf(buf, MAX_BUF_LEN, message, arg_list);

    fprintf(is_error ? stderr : stdout, "\033[%im%s\033[0m\n", is_error ? 91 : 90, buf);
}

/**
 * @brief Prints a formatted error message into the console. This version uses some colours to make the message appear more visually appealing.
 * @param file The file the error message came from
 * @param function The function the error message came from
 * @param line The line in the file the error message came from
 * @param err_msg The error message to send into the console
 * @param err_details Any extra details a user my want to send alongside the error message in the console.
 * @param type The type of error to log to the console
 */
void LinuxLogger::log_error(const char *file, const char *function, int line, const char *err_msg, const char *err_details, ErrorType type) {
    int colour_value = 31;
    const char *console_msg;

    switch(type) {
        case ERROR_ERR: {
            colour_value = 31;
            console_msg = "ERROR";
        } break;
        case ERROR_WARNING: {
            colour_value = 93;
            console_msg = "WARNING";
        } break;
        case ERROR_DEBUG: {
            colour_value = 94;
            console_msg = "DEBUG";
        } break;
        case ERROR_SHADER: {
            colour_value = 95;
            console_msg = "SHADER ERROR";
        } break;
    }

    logf(true, "\033[%im%s:\033[%im %s %s \033[0m", colour_value, console_msg, colour_value - 60, err_msg, err_details);
    logf(false, "\033[90m    at: %s <%s:%i>\033[0m", file, function, line);
}

#endif // PLATFORM_LINUX