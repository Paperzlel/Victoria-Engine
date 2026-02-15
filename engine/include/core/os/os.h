#pragma once

#include <core/config/main_loop.h>
#include <core/error/error_macros.h>
#include <core/io/logger.h>
#include <core/string/vstring.h>


class VAPI OS {
protected:

    static OS *singleton;

    Logger *_logger = nullptr;

    u8 exit_code = 0;

    u64 timer_frequency = 0;
    u64 timer_offset = 0;

    bool stdout_verbose = false;
    bool suspended = false;

    String device_vendor;
    String device_name;
public:

    static OS *get_singleton();

    u8 get_exit_code() const;
    void set_exit_code(u8 p_id);

    bool is_suspended() const;
    void set_is_suspended(bool p_value);

    u64 get_timer_frequency() { return timer_frequency; }

    bool is_stdout_verbose() const;
    void set_stdout_verbose(bool p_value);

    virtual void alert(const char *message, const char *title) const = 0;

    virtual void print(const char *message, ...) = 0;
    virtual void print_error(const char *file, const char *function, int line, const char *err_msg, const char *extra_msg = "", ErrorType type = ERROR_ERR) = 0;
    virtual void printerr(const char *message, ...) = 0;

    virtual u64 get_current_time_usec() = 0;
    virtual double get_os_running_time() = 0;

    virtual bool has_feature(const String &p_feature) const = 0;
    virtual void set_feature(const String &p_feature, bool p_value) = 0;

    virtual String get_environment(const String &p_name) const = 0;
    virtual void set_environment(const String &p_name, const String &p_value) = 0;

    virtual String get_name() const = 0;
    virtual String get_version() const = 0;

    virtual String get_executable_path() const = 0;

    virtual String get_device_name() const { return device_name; }
    virtual String get_device_vendor() const { return device_vendor; }

    virtual void set_device_name(const String &p_name) { device_name = p_name; }
    virtual void set_device_vendor(const String &p_vendor) { device_vendor = p_vendor; }

    virtual int get_preferred_display_manager() = 0;

    virtual MainLoop *get_main_loop() const = 0;
    virtual void set_main_loop(MainLoop *p_tree) = 0;
    virtual void delete_main_loop() = 0;

    virtual void initialize() = 0;
    virtual void run() = 0;

    OS();
    ~OS();
};
