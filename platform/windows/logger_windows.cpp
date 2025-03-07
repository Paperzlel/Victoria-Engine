#include "logger_windows.h"
#if PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Method to log a given message with a specific `va_list` given. This implementation attempts to colour the text for readability, using either ANSI escape codes or Windows console text attributes.
 * @param is_error Option to specify if we flag the message as an error (red text) or as a normal message (cyan)
 * @param message The message we want to format and send to the console
 * @param arg_list The `va_list` we want to format its arguments into the message
 */
void WindowsLogger::logv(bool is_error, const char *message, va_list arg_list) {
    if (!_can_use_vt_printing) {
        // Can't use ANSI escape codes, so use console text attribs instead
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

        if (!console || console == INVALID_HANDLE_VALUE) {
            Logger::logv(is_error, message, arg_list);
        } else {
            uint8 colour_attrib = is_error ? 1 : 8;

            SetConsoleTextAttribute(console, colour_attrib);
            vfprintf(stdout, message, arg_list);
            // Add a newline manually for debug output.
            fprintf(stdout, "\n");
            SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
        }
    } else {
        char buf[MAX_BUF_LEN];
        memset(buf, 0, MAX_BUF_LEN);

        vsnprintf(buf, MAX_BUF_LEN, message, arg_list);

        fprintf(is_error ? stderr : stdout,"\033[%im%s\033[0m\n", is_error ? 91 : 90, buf);
    }
}

/**
 * @brief Method to log an error to the Windows terminal. Differs from `logv` with `is_error` set to true as it takes the file, function and line as inputs, as well as some different formatting to make the errors more readable.
 * @param file The file the error comes from
 * @param function The function the error comes from
 * @param line The line the error comes from
 * @param err_msg The error message we want to specify
 * @param err_details The details of a given error
 * @param type The type of error to log to the console
 */
void WindowsLogger::log_error(const char *file, const char *function, int line, const char *err_msg, const char *err_details, ErrorType type) {
    if (!_can_use_vt_printing) {
        // The logger can't deal with specific formatting so we send it off to logv instead
        Logger::logf(true, "ERROR: %s %s", err_msg, err_details);
        Logger::logf(false, "    at: %s <%s:%i>", file, function, line);
    } else {

        int colour_value = 91;
        const char *console_msg;

        switch (type) {
            case ERROR_ERR: {
                colour_value = 91;
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

        logf(true, "\033[%im%s:\033[%im %s %s\033[0m", colour_value, console_msg, colour_value - 60, err_msg, err_details);
        logf(false, "\033[90m    at: %s <%s:%i>\033[0m", file, function, line);
    }
}

WindowsLogger::WindowsLogger(bool use_vt) {
    _can_use_vt_printing = use_vt;
}

#endif // PLATFORM_WINDOWS