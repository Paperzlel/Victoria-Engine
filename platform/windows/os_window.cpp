#include "os_windows.h"
#if PLATFORM_WINDOWS

#include "logger_windows.h"
#include "core/os/display_manager.h"
#include "main/main.h"


/**
 * @brief Method that makes a message box with information appear, and pauses the current thread.
 * @param message The message to enter in the message box
 * @param title The title of the message box
 */
void OSWindows::alert(const char *message, const char *title) const {
    MessageBoxA(nullptr, message, title, MB_ICONEXCLAMATION | MB_OK);
}

/**
 * @brief Method that prints a regular message to the current terminal.
 * @param message The message (as a string) to print in the terminal
 * @param ... Variadic arguments to format into the message
 */
void OSWindows::print(const char *message, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, message);
    
    if (_logger) {
        _logger->logv(false, message, arg_ptr);
    }
    
    va_end(arg_ptr);
}

/**
 * @brief Method to print a formatted error message into the terminal
 * @param file The current file that the error message resides in, should be represented by a macro
 * @param function The function that the error is calling from
 * @param line The line number of the error message 
 * @param err_msg The error message to display what went wrong within the terminal
 * @param extra_msg An extra message to specify any other details one needs to clarify
 */
void OSWindows::print_error(const char *file, const char *function, int line, const char *err_msg, const char *extra_msg, ErrorType type) {
    if (_logger) {
        _logger->log_error(file, function, line, err_msg, extra_msg, (Logger::ErrorType)type);
    }
}

/**
 * @brief Method to print an error message to the terminal, with only text highlighting
 * @param message The message to print in the terminal
 * @param ... Variadic arguments to format into the message itself
 */
void OSWindows::printerr(const char *message, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, message);
    
    if (_logger) {
        _logger->logv(true, message, arg_ptr);
    }
    va_end(arg_ptr);
}

uint64 OSWindows::get_current_time_usec() {
    uint64 ticks;
    QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
    return ticks;
}

double OSWindows::get_os_running_time() {
    return (double) (get_current_time_usec() - timer_offset) / get_timer_frequency();
}

/**
 * @brief Method that initializes the OS class.
 * @return `OK` if all events were able to be fired, and various fail cases if not.
 */
void OSWindows::initialize() {
    if (!_hInstance) {
        _hInstance = GetModuleHandleA(NULL);
    }

    QueryPerformanceFrequency((LARGE_INTEGER *)&timer_frequency);
    timer_offset = get_current_time_usec();
}

void OSWindows::run() {
    while (true) {
        DisplayManager::get_singleton()->process_events();
        if (iteration()) {
            break;
        }
    }
}

OSWindows::OSWindows(HINSTANCE hInstance) {
    _hInstance = hInstance;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    bool use_vt = SetConsoleMode(hOut, dwMode);

    _logger = new WindowsLogger(use_vt);
}

OSWindows::~OSWindows() {
    delete _logger;
}

#endif // PLATFORM_WINDOWS