#pragma once

#include "error_types.h"
#include "core/typedefs.h"

enum ErrorType {
    ERROR_ERR,
    ERROR_WARNING,
    ERROR_DEBUG,
    ERROR_SHADER,
};

/** 
 * @brief Prints an error to the console.
 * @param file The current file the macro is in
 * @param function The current function the macro was executed from
 * @param line The line number the macro is on
 * @param message The message to display to the console
 */
void _err_print_err(const char *file, const char *function, int line, const char *err_msg, const char *msg = "", ErrorType p_type = ERROR_ERR);

void _err_print_index_err(const char *file, const char *function, int line, int m_index, int m_size, const char *err_msg, const char *msg = "", ErrorType p_type = ERROR_ERR);

#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__)
#define DEBUG_BREAK() __builtin_debugtrap()
#else
#define DEBUG_BREAK() 
#endif

#define FUNCTION_STR __FUNCTION__

#define cond(x) x

/**
 * Error macros for an out of bounds index within an array
 */

#define ERR_OUT_OF_BOUNDS(m_index, m_size)                                                                              \
    if (m_index < 0 || m_index > m_size) {                                                                              \
        _err_print_index_err(__FILE__, FUNCTION_STR, __LINE__, m_index, m_size, "Index given was out of bounds.");      \
        return;                                                                                                         \
    }                                                                                                                   \

#define ERR_OUT_OF_BOUNDS_MSG(m_index, m_size, m_msg)                                                                       \
    if (m_index < 0 || m_index > m_size) {                                                                                  \
        _err_print_index_err(__FILE__, FUNCTION_STR, __LINE__, m_index, m_size, "Index given was out of bounds." m_msg)     \
        return;                                                                                                             \
    }                                                                                                                       \

#define ERR_OUT_OF_BOUNDS_R(m_index, m_size, m_retval)                                                                                              \
    if (m_index < 0 || m_index > m_size) {                                                                                                          \
        _err_print_index_err(__FILE__, __FUNCTION__, __LINE__, m_index, m_size, "Index given was out of bounds. Returning " _STR(m_retval) ".");    \
        return m_retval;                                                                                                                            \
    }                                                                                                                                               \

#define ERR_OUT_OF_BOUNDS_MSG_R(m_index, m_size, m_msg, m_retval)                                                                                           \
    if (m_index < 0 || m_index > m_size) {                                                                                                                  \
        _err_print_index_err(__FILE__, __FUNCTION__, __LINE__, m_index, m_size, "Index given was out of bounds. Returning " _STR(m_retval) ".", m_msg);     \
        return m_retval;                                                                                                                                    \
    }                                                                                                                                                       \

/**
 * @brief Macro to throw an exception whenever an index is out of bounds. To be used in scenarios where an index can be out of bounds, but the return value is non-void and cannot be cast to 0.
 * @param m_index The index being requested
 * @param m_size The number of elements in the array
 */
#define ERR_OUT_OF_BOUNDS_FATAL(m_index, m_size)                                                                        \
    if (m_index < 0 || m_index > m_size) {                                                                              \
        _err_print_index_err(__FILE__, FUNCTION_STR, __LINE__, m_index, m_size, "Index given was out of bounds.");      \
        DEBUG_BREAK();                                                                                                  \
    }                                                                                                                   \


/**
 * Error messages that report when the given condition is true
 */

#define ERR_FAIL_COND(m_cond)                                                                           \
    if (cond(m_cond)) {                                                                                 \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" was true.");   \
        return;                                                                                         \
    }                                                                                                   \

#define ERR_FAIL_COND_MSG(m_cond, m_msg)                                                                        \
    if (cond(m_cond)) {                                                                                         \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" was true.", m_msg);    \
        return;                                                                                                 \
    }                                                                                                           \

#define ERR_FAIL_COND_R(m_cond, retval)                                                                                             \
    if (cond(m_cond)) {                                                                                                             \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" was true. Returning " _STR(retval) ".");   \
        return retval;                                                                                                              \
    }                                                                                                                               \

#define ERR_FAIL_COND_MSG_R(m_cond, m_msg, retval)                                                                                          \
    if (cond(m_cond)) {                                                                                                                     \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" was true. Returning " _STR(retval) ".", m_msg);    \
        return retval;                                                                                                                      \
    }                                                                                                                                       \

/**
 * Error messages that report when an item is null
 */

#define ERR_COND_NULL(m_item)                                                                       \
    if (cond(m_item == nullptr)) {                                                                  \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Item \"" _STR(m_item) "\" is null.");     \
        return;                                                                                     \
    }                                                                                               \

#define ERR_COND_NULL_MSG(m_item, m_msg)                                                                \
    if (cond(m_item == nullptr)) {                                                                      \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Item \"" _STR(m_item) "\" is null.", m_msg);   \
        return;                                                                                         \
    }                                                                                                   \

#define ERR_COND_NULL_R(m_item, retval)                                                                                         \
    if (cond(m_item == nullptr)) {                                                                                              \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Item \"" _STR(m_item) "\" is null. Returning " _STR(retval) ".");     \
        return retval;                                                                                                          \
    }                                                                                                                           \

#define ERR_COND_NULL_MSG_R(m_item, m_msg, retval)                                                                                      \
    if (cond(m_item == nullptr)) {                                                                                                      \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Item \"" _STR(m_item) "\" is null. Returning " _STR(retval) ".", m_msg);      \
        return retval;                                                                                                                  \
    }                                                                                                                                   \

/**
 * @brief Macro to define a fatal error within the application when it can no longer continue.
 * We return `retval` which will normally be null, however due to the `DEBUG_BREAK()` macro it will pause the application regardless.
 * @param m_cond The condition to check. If it is true, it will trigger the macro.
 * @param retval The value to return in case of a fatal error.
 * @return `retval`.
 */
#define ERR_COND_FATAL(m_cond)                                                                                                      \
    if (cond(m_cond)) {                                                                                                             \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" is true, a fatal error has occured.");     \
        DEBUG_BREAK();                                                                                                              \
    }                                                                                                                               \

