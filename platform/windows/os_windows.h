#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS
#include "core/os/os.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class OSWindows : public OS {
private:

    HINSTANCE _hInstance = nullptr;
public:

    virtual void alert(const char *message, const char *title) const override;

    virtual void print(const char *message, ...) override;
    virtual void print_error(const char *file, const char *function, int line, const char *err_msg, const char *extra_msg, ErrorType type) override;
    virtual void printerr(const char *message, ...) override;

    virtual uint64 get_current_time_usec() override;
    virtual double get_os_running_time() override;

    virtual void initialize() override;
    virtual void run() override;


    HINSTANCE get_hinstance() { return _hInstance; }
    OSWindows(HINSTANCE hInstance);
    ~OSWindows(); // No current shutdown methods
};

#endif // PLATFORM_WINDOWS