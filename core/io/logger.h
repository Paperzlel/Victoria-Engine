#ifndef LOGGER_H
#define LOGGER_H

class Logger {
protected:
    static Logger *singleton;

public:
    static const int MAX_BUF_LEN = 32000;

    static Logger *get_singleton();

    void logv(bool is_error, const char *message, __builtin_va_list va_list);

    void logf(bool is_error, const char *message, ...);

    void log_debug_v(const char *file, const char *function, int line, const char *message, ...);

    Logger();
};

#define PRINT(message, ...) Logger::get_singleton()->logf(false, message, ##__VA_ARGS__)

#define PRINT_DEBUG(message, ...) Logger::get_singleton()->log_debug_v(__FILE__, __FUNCTION__, __LINE__, message, ##__VA_ARGS__)

#define PRINT_ERR(message, ...) Logger::get_singleton()->logf(true, message, ##__VA_ARGS__)

#endif //LOGGER_H