#pragma once

#include "core/typedefs.h"
#if PLATFORM_LINUX

#include "core/os/os.h"

class OSLinux : public OS {
public:

    virtual void alert(const char *message, const char *title) const override;

    virtual void print(const char *message, ...) override;
    virtual void print_error(const char *file, const char *function, int line, const char *err_msg, const char *extra_msg, ErrorType type) override;
    virtual void printerr(const char *message, ...) override;

    virtual uint64 get_current_time_usec() override;
    virtual double get_os_running_time() override;

    virtual void initialize() override;
    virtual void run() override;
    
    OSLinux();
    ~OSLinux();
};

#endif // PLATFORM_LINUX