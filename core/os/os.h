#pragma once

#include "core/typedefs.h"
#include "core/data/event.h"
#include "core/error/error_macros.h"
#include "core/io/logger.h"

class OS {
protected:

    // Singleton instance for the `OS` class.
    static OS *singleton;

    // Reference to the logger class used by the OS for printing.
    Logger *_logger = nullptr;

    // The current exit code to use when shutting down the engine.
    u8 exit_code = 0;

    // The number of timer ticks per second.
    u64 timer_frequency = 0;
    // The offset from when the timer began for the OS.
    u64 timer_offset = 0;
public:

    /**
     * @brief Obtains the singleton class instance for the OS.
     */
    static OS *get_singleton();

    /**
     * @brief Getter for the exit code. Only used on event shutdown.
     * @return The exit code for the application
     */
    u8 get_exit_code() const;
    /**
     * @brief Setter for the exit code. Set this if an error has occured.
     * @param p_id The exit ID to set.
     */
    void set_exit_code(u8 p_id);

    /**
     * @brief Getter for the timer frequency.
     * @return The timer frequency
     */
    u64 get_timer_frequency() { return timer_frequency; }

    /**
     * @brief Opens an alert box on the user's end and displays a message inside it, blocking the current thread until closed. 
     * @param message The message to display
     * @param title The title of the alert box
     */
    virtual void alert(const char *message, const char *title) const = 0;

    /**
     * @brief Prints a given message to the console, with variadic arguments that can be applied. Does not use in-engine string
     * formatting.
     * @param message The message to print
     * @param ... Variadic options to format into the message
     */
    virtual void print(const char *message, ...) = 0;
    /**
     * @brief Prints an error message to the console, with color options applied depending on the type of error.
     * @param file The file where the error happened
     * @param function The function where the error happened
     * @param line The line where the error happened
     * @param err_msg The error message to displau
     * @param extra_msg Extra information about the error message to display
     * @param type The type of error to display (see `ErrorType`)
     */
    virtual void print_error(const char *file, const char *function, int line, const char *err_msg, const char *extra_msg = "", ErrorType type = ERROR_ERR) = 0; 
    /**
     * @brief Prints an error message to the console that has no specific cause. This is only to be used in situations where an error
     * must be emitted but the location is not significant, or if the user wishes to print an error.
     * @param message The message to display
     * @param ... Variadic macros that can be formatted into the message string
     */
    virtual void printerr(const char *message, ...) = 0;

    /**
     * @brief Obtains the time since launch of the operating system or the Unix timestamp of the system. Implementation
     * depends on the platform.
     * @return The time since operating system startup or its UNIX timestamp.
     */
    virtual u64 get_current_time_usec() = 0;
    /**
     * @brief Obtains the amount of time the engine has been running for, in seconds.
     * @returns The runtime of the OS in seconds
     */
    virtual double get_os_running_time() = 0;

    /**
     * @brief Initializes all the basics of the underlying OS (all sorts of drivers and such) for the platform it was compiled for.
     */
    virtual void initialize() = 0;
    /**
     * @brief Enters the infinite loop of the application, where events are processed and the application is updated constantly. This
     * only returns once the user has requested to quit or a close was required for some reason.
     */
    virtual void run() = 0;

    /**
     * @brief Operating system class constructor.
     */
    OS();
    /**
     * @brief OS class destructor.
     */
    ~OS();
};