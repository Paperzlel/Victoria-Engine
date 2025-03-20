#include "os_linux.h"
#if PLATFORM_LINUX

#include "logger_linux.h"
#include "display_manager_x11.h"

#include "main/main.h"


#include <stdarg.h>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#else
#error "Require a POSIX-compliant system of a high enough version!"
#endif

/**
 * @brief Creates an alert box to display an important message to the user. Please note that this feature is not supported properly on Linux, as there
 * is no equivalent to `MessageBox` for the X11/Wayland API without using a third-party library or creating our own.
 * @param message The message to display in the box
 * @param title The title of the alert box
 */
void OSLinux::alert(const char *message, const char *title) const {
    OS::get_singleton()->printerr("%s \n%s", title, message);
}

/**
 * @brief Prints a message to the currently active console.
 * @param message The message to print
 * @param ... Variadic arguments to format into the message
 */
void OSLinux::print(const char *message, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, message);
    
    if (_logger) {
        _logger->logv(false, message, arg_ptr);
    }
    
    va_end(arg_ptr);
}

/**
 * @brief Prints an error message from where it was called in the application to the currently active console.
 * @param file The file the error comes from
 * @param function The function the error comes from
 * @param line The line in the file the error comes from
 * @param err_msg The error message to output
 * @param extra_msg Any extra details about the error to give the user
 */
void OSLinux::print_error(const char *file, const char *function, int line, const char *err_msg, const char *extra_msg, ErrorType type) {
    if (_logger) {
        _logger->log_error(file, function, line, err_msg, extra_msg, (Logger::ErrorType)type);
    }
}

/**
 * @brief Prints a message to the console highlighted as an error.
 * @param message The message to send to the console
 * @param ... Variadic arguments to format into the message
 */
void OSLinux::printerr(const char *message, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, message);
    
    if (_logger) {
        _logger->logv(true, message, arg_ptr);
    }
    
    va_end(arg_ptr);
}

uint64 OSLinux::get_current_time_usec() {
    timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return (uint64) (t.tv_nsec / 1000) + t.tv_sec * 1000000; // Return it to 1us rather than ns
}

double OSLinux::get_os_running_time() {
    return (double) (get_current_time_usec() - timer_offset) / 1000000;
}

/**
 * @brief Intialization phase for the OS class.
 */
void OSLinux::initialize() {
    timespec t;
    clock_getres(CLOCK_MONOTONIC, &t);
    timer_frequency = t.tv_nsec * 1000000000; // NOTE: Should always be ~1ns anyway :/
    timer_offset = get_current_time_usec();
}

void OSLinux::run() {

    while (true) {
        DisplayManager::get_singleton()->process_events();
        if (iteration()) {
            break;
        }
    }
}

/**
 * @brief Class constructor for the Linux operating system. 
 */
OSLinux::OSLinux() {
    _logger = new LinuxLogger();
}

/**
 * @brief Class destructor for the Linux operating system.
 */
OSLinux::~OSLinux() {
    delete _logger;
}

#endif // PLATFORM_LINUX