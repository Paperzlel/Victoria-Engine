#include "logger_windows.h"
#if PLATFORM_WINDOWS

#	include "core/string/vstring.h"

#	define WIN32_LEAN_AND_MEAN
#	include <stdio.h>
#	include <string.h>
#	include <windows.h>

/**
 * @brief Method to log a given message with a specific `va_list` given. This implementation attempts to colour the
 * text for readability, using either ANSI escape codes or Windows console text attributes.
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
			uint8_t colour_attrib = is_error ? 1 : 8;

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

		fprintf(is_error ? stderr : stdout, "\033[%im%s\033[0m\n", is_error ? 91 : 90, buf);
	}
}

/**
 * @brief Method to log an error to the Windows terminal. Differs from `logv` with `is_error` set to true as it takes
 * the file, function and line as inputs, as well as some different formatting to make the errors more readable.
 * @param file The file the error comes from
 * @param function The function the error comes from
 * @param line The line the error comes from
 * @param err_msg The error message we want to specify
 * @param err_details The details of a given error
 * @param type The type of error to log to the console
 */
void WindowsLogger::log_error(const char *file,
							  const char *function,
							  int line,
							  const char *err_msg,
							  const char *err_details,
							  ErrorType type) {
	if (!_can_use_vt_printing) {
		// The logger can't deal with specific formatting so we send it off to logv instead
		Logger::logf(true, "ERROR: %s %s", err_msg, err_details);
		Logger::logf(false, "    at: %s <%s:%i>", function, file, line);
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

		logf(true,
			 "\033[%im%s:\033[%im %s %s\033[0m",
			 colour_value,
			 console_msg,
			 colour_value - 60,
			 err_msg,
			 err_details);
		logf(false, "\033[90m    at: %s <%s:%i>\033[0m", function, file, line);
	}
}

/**
 * @brief Class constructor for the Windows terminal logger
 * @param use_vt Whether or not we can use ANSI escape codes for our terminal, which is checked during OS
 * initialization
 */
WindowsLogger::WindowsLogger(bool use_vt) {
	_can_use_vt_printing = use_vt;
}

// TEMP

/**
 * General escape code is \033[<value>m
 */

/**
 * ANSI ESCAPE CODES: COLOURS
 * 0 - Default
 * 1 - Bold
 * 22 - Remove bold
 * 4 - Underline
 * 24 - Remove underline
 * 7 - Negative (swap background and foreground)
 * 27 - Return foreground/background to normal
 * 30 - Foreground Black
 * 31 - Foreground Red
 * 32 - Foreground Green
 * 33 - Foreground Yellow
 * 34 - Foreground Blue
 * 35 - Foreground Magenta
 * 36 - Foreground Cyan
 * 37 - Foreground White
 * 38 - Foreground Extended (see next table)
 * 39 	Foreground Default 	Applies only the foreground portion of the defaults (see 0)
 * 40 	Background Black 	Applies non-bold/bright black to background
 * 41 	Background Red 	Applies non-bold/bright red to background
 * 42 	Background Green 	Applies non-bold/bright green to background
 * 43 	Background Yellow 	Applies non-bold/bright yellow to background
 * 44 	Background Blue 	Applies non-bold/bright blue to background
 * 45 	Background Magenta 	Applies non-bold/bright magenta to background
 * 46 	Background Cyan 	Applies non-bold/bright cyan to background
 * 47 	Background White 	Applies non-bold/bright white to background
 * 48 	Background Extended 	Applies extended color value to the background (see details below)
 * 49 	Background Default 	Applies only the background portion of the defaults (see 0)
 * 90 	Bright Foreground Black 	Applies bold/bright black to foreground
 * 91 	Bright Foreground Red 	Applies bold/bright red to foreground
 * 92 	Bright Foreground Green 	Applies bold/bright green to foreground
 * 93 	Bright Foreground Yellow 	Applies bold/bright yellow to foreground
 * 94 	Bright Foreground Blue 	Applies bold/bright blue to foreground
 * 95 	Bright Foreground Magenta 	Applies bold/bright magenta to foreground
 * 96 	Bright Foreground Cyan 	Applies bold/bright cyan to foreground
 * 97 	Bright Foreground White 	Applies bold/bright white to foreground
 * 100 	Bright Background Black 	Applies bold/bright black to background
 * 101 	Bright Background Red 	Applies bold/bright red to background
 * 102 	Bright Background Green 	Applies bold/bright green to background
 * 103 	Bright Background Yellow 	Applies bold/bright yellow to background
 * 104 	Bright Background Blue 	Applies bold/bright blue to background
 * 105 	Bright Background Magenta 	Applies bold/bright magenta to background
 * 106 	Bright Background Cyan 	Applies bold/bright cyan to background
 * 107 	Bright Background White 	Applies bold/bright white to background
 */

/**
 * ANSI ESCAPE CODE COLOURS:
 * 38 ; 2 ; <r> ; <g> ; <b> 	Set foreground color to RGB value specified in <r>, <g>, <b> parameters*
 * 48 ; 2 ; <r> ; <g> ; <b> 	Set background color to RGB value specified in <r>, <g>, <b> parameters*
 * 38 ; 5 ; <s> 	Set foreground color to <s> index in 88 or 256 color table*
 * 48 ; 5 ; <s> 	Set background color to <s> index in 88 or 256 color table*
 */
// TEMP

#endif // PLATFORM_WINDOWS