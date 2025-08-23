#pragma once

#include "core/typedefs.h"
#include "core/data/event.h"
#include "core/error/error_macros.h"
#include "core/io/logger.h"

class OS {
protected:

    static OS *singleton;

    Logger *_logger = nullptr;

    u8 exit_code = 0;

    u64 timer_frequency = 0;
    u64 timer_offset = 0;
public:

    static OS *get_singleton();

    u8 get_exit_code() const;
    void set_exit_code(u8 p_id);

    u64 get_timer_frequency() { return timer_frequency; }

    virtual void alert(const char *message, const char *title) const = 0;

    virtual void print(const char *message, ...) = 0;
    virtual void print_error(const char *file, const char *function, int line, const char *err_msg, const char *extra_msg = "", ErrorType type = ERROR_ERR) = 0; 
    virtual void printerr(const char *message, ...) = 0;

    virtual u64 get_current_time_usec() = 0;
    virtual double get_os_running_time() = 0;

    virtual void initialize() = 0;
    virtual void run() = 0;

    OS();
    ~OS();
};