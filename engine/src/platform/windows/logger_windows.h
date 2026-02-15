#pragma once

#include <core/typedefs.h>

#if PLATFORM_WINDOWS
#include <core/io/logger.h>

/**
 * @brief Class that outputs to the Windows Terminal, using coloured text for messages.
 */
class WindowsLogger : public Logger {
private:
    bool _can_use_vt_printing = false;
public:
    virtual void logv(bool is_error, const char *message, va_list arg_list) override;
    virtual void log_error(const char *file, const char *function, int line, const char *err_msg, const char *err_details, ErrorType type) override;

    WindowsLogger(bool use_vt);
};

#endif // PLATFORM_WINDOWS