#include "os_windows.h"
#if PLATFORM_WINDOWS

#include "logger_windows.h"
#include "display_manager_windows.h"
#include "main/main.h"

typedef int (WINAPI *RtlGetVersion)(LPOSVERSIONINFOEXA);

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


u64 OSWindows::get_current_time_usec() {
    u64 ticks;
    QueryPerformanceCounter((LARGE_INTEGER *)&ticks);
    return ticks;
}

double OSWindows::get_os_running_time() {
    return (double) (get_current_time_usec() - timer_offset) / get_timer_frequency();
}

bool OSWindows::has_feature(const String &p_feature) const {
    return features[p_feature];
}

void OSWindows::set_feature(const String &p_feature, bool p_value) {
    features[p_feature] = p_value;
}

String OSWindows::get_name() const {
    return "Windows";
}

String OSWindows::get_version() const {
    RtlGetVersion victoria_getv = nullptr;
    victoria_getv = (RtlGetVersion)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
    if (victoria_getv) {
        OSVERSIONINFOEXA os_info;
        ZeroMemory(&os_info, sizeof(OSVERSIONINFOEXA));
        victoria_getv(&os_info);
        return vformat("%d.%d.%d", os_info.dwMajorVersion, os_info.dwMinorVersion, os_info.dwBuildNumber);
    }
    return "";
}

String OSWindows::get_executable_path() const {
    char exe_path[MAX_PATH];
    GetModuleFileNameA(nullptr, exe_path, MAX_PATH);
    exe_path[MAX_PATH - 1] = 0;
    return exe_path;
}

MainLoop *OSWindows::get_main_loop() const {
    return main_loop;
}

void OSWindows::set_main_loop(MainLoop *p_tree) {
    main_loop = p_tree;
}

void OSWindows::delete_main_loop() {
    if (main_loop) {
        vdelete(main_loop);
    }
    
    main_loop = nullptr;
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

    DisplayManagerWindows::register_windows_driver();
}

/**
 * @brief Method that contains the main loop of the application. Main's version of this runs somewhat differently, in that it runs an iteration of the system's loop, and is the point by which the application can actually terminate.
 */
void OSWindows::run() {
    if (!main_loop) {
        return;
    }

    main_loop->initialize();

    while (true) {
        DisplayManager::get_singleton()->process_events();
        if (Main::iteration()) {
            break;
        }
    }
}

/**
 * @brief Class constructor for the OS class. Initializes the instance, and checks to see if we can use virtual terminal processing, or in other words, if we can use Unix-like coloured text in the terminal.
 * @param hInstance The instance of the current application
 */
OSWindows::OSWindows(HINSTANCE hInstance) {
    _hInstance = hInstance;
    // Setting up the console to print ANSI/VT100 escape codes
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    
    // Checks if we can use virtual terminal processing. If not, we use Windows' custom color schemes
    bool use_vt = SetConsoleMode(hOut, dwMode);
    set_feature("ansi_escape_codes", use_vt);

    // By default, we want somewhere for printing to go to, and we implement that here.
    // This does mean that to some extent Logger is also abstract, but it functions as a fallback for some methods.
    _logger = vnew(WindowsLogger(use_vt));
}

OSWindows::~OSWindows() {
    vdelete(_logger);
}

#endif //PLATFORM_WINDOWS