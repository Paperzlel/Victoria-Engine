#pragma once

#include "core/typedefs.h"
#if PLATFORM_LINUX

#include "core/io/logger.h"

class LinuxLogger : public Logger {
public:

    virtual void logv(bool is_error, const char *message, va_list arg_list) override;
    virtual void log_error(const char *file, const char *function, int line, const char *err_msg, const char *err_details, ErrorType type) override;
};

#endif // PLATFORM_LINUX