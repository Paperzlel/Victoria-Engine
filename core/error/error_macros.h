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

/**
 * @brief Prints an index error (when a given item was out of bounds) to the currently active console.
 * @param file The file the macro is currently in
 * @param function The function the macro was executed from
 * @param line The line the macro was on
 * @param m_index The given index into the array
 * @param m_size The element count of the current array
 * @param err_msg The primary error message to send to the console (usually along the lines of `"Given index was out of bounds."`)
 * @param msg An (optional) message to send to the console if the given error requires extra details to be shown to the user
 * @param type The type of error message that is being sent
 */
void _err_print_index_err(const char *file, const char *function, int line, int m_index, int m_size, const char *err_msg, const char *msg = "", ErrorType p_type = ERROR_ERR);

#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__)
#define DEBUG_BREAK() __builtin_debugtrap()
#else
#define DEBUG_BREAK() 
#endif

// Wrapper for the function-finding macro that doesn't highlight properly in some IDEs.
#define FUNCTION_STR __FUNCTION__


// Wrapper to make it so that a condition given can be expressed properly. Some evaluations do not work without this.
#define cond(x) x

/**
 * Error macros for an out of bounds index within an array
 */

/**
 * @brief Throws an error whenever the given index is out of bounds and returns nothing. Should only be used where there is no suitable message to give the user, 
 * and when the current function does not have a return value.
 * @param m_index The current index into the array
 * @param m_size The total element count of the array
 */
#define ERR_OUT_OF_BOUNDS(m_index, m_size)                                                                              \
    if (m_index < 0 || m_index > m_size) {                                                                              \
        _err_print_index_err(__FILE__, FUNCTION_STR, __LINE__, m_index, m_size, "Index given was out of bounds.");      \
        return;                                                                                                         \
    }                                                                                                                   \

/**
 * @brief Macro that throws an error whenever the given index is out of bounds and returns nothing, as well as an extra message for the user to understand what 
 * went wrong. Should be used in situations when there is no suitable return value.
 * @param m_index The given index into the array
 * @param m_size The total element count of the array
 * @param m_msg The error message to send to the console when the given index is out of bounds
 */
#define ERR_OUT_OF_BOUNDS_MSG(m_index, m_size, m_msg)                                                                       \
    if (m_index < 0 || m_index > m_size) {                                                                                  \
        _err_print_index_err(__FILE__, FUNCTION_STR, __LINE__, m_index, m_size, "Index given was out of bounds." m_msg)     \
        return;                                                                                                             \
    }                                                                                                                       \

/**
 * @brief Throws an error whenever the given index is out of bounds and returns a given value. Should only be used in situations where there is no suitable error 
 * message to notify the user of. Generally speaking, the return value of this macro should be something unexpected or unique that the user can handle properly in 
 * their code.
 * @param m_index The given index into the array
 * @param m_size The total element count of the array
 * @param m_retval The return value for the current function
 * @returns Whatever value m_retval is set to
 */
#define ERR_OUT_OF_BOUNDS_R(m_index, m_size, m_retval)                                                                                              \
    if (m_index < 0 || m_index > m_size) {                                                                                                          \
        _err_print_index_err(__FILE__, __FUNCTION__, __LINE__, m_index, m_size, "Index given was out of bounds. Returning " _STR(m_retval) ".");    \
        return m_retval;                                                                                                                            \
    }                                                                                                                                               \

/**
 * @brief Throws an error whenever the given index is out of bounds and returns a given value, as well as sending an extra message to the user to understand what 
 * went wrong. Should be used in situations where you have both a return value and extra details for the user to handle. Generally speaking, the return value of 
 * this macro should be something unexpected or unique that the user can handle properly in their code.
 * @param m_index The given index into the array
 * @param m_size The total element count of the array
 * @param m_msg The error message to send to the console
 * @param m_retval The return value for the current function
 * @returns Whatever value `m_retval` is set to
 */
#define ERR_OUT_OF_BOUNDS_MSG_R(m_index, m_size, m_msg, m_retval)                                                                                           \
    if (m_index < 0 || m_index > m_size) {                                                                                                                  \
        _err_print_index_err(__FILE__, __FUNCTION__, __LINE__, m_index, m_size, "Index given was out of bounds. Returning " _STR(m_retval) ".", m_msg);     \
        return m_retval;                                                                                                                                    \
    }                                                                                                                                                       \

/**
 * @brief Throws an error whenever the given index is out of bounds. This macro does not return early, but instead calls to `DEBUG_BREAK()` to add a breakpoint at 
 * the current function. Should only ever be used in a situation where the error the program currently faces cannot allow for the application to continue, and 
 * needs to be inspected carefully. 
 * @param m_index The given indexs into the array
 * @param m_size The total element count of the array
 */
#define ERR_OUT_OF_BOUNDS_FATAL(m_index, m_size)                                                                        \
    if (m_index < 0 || m_index > m_size) {                                                                              \
        _err_print_index_err(__FILE__, FUNCTION_STR, __LINE__, m_index, m_size, "Index given was out of bounds.");      \
        DEBUG_BREAK();                                                                                                  \
    }                                                                                                                   \


/**
 * Error messages that report when the given condition is true
 */

/**
 * @brief Throws an error whenever the given condition is true. Should only be used when there is no suitable return value nor a suitable enough message to output 
 * into the console for the user. 
 * @param m_cond The given condition to assess
 */
#define ERR_FAIL_COND(m_cond)                                                                           \
    if (cond(m_cond)) {                                                                                 \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" was true.");   \
        return;                                                                                         \
    }                                                                                                   \

/**
 * @brief Throws an error whenever the given condition is true, as well as sending an extra error message to the user to understand what went wrong. Should be 
 * used in situations when there is no suitable return value for the function.
 * @param m_cond The given condition to assess
 * @param m_msg The given error message to send to the console when the given condition is true
 */
#define ERR_FAIL_COND_MSG(m_cond, m_msg)                                                                        \
    if (cond(m_cond)) {                                                                                         \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" was true.", m_msg);    \
        return;                                                                                                 \
    }                                                                                                           \

/**
 * @brief Throws an error whenever the given condition is true and returns a given value. Should only be used in situations when there is no suitable extra 
 * message to send to the user. Generally speaking, the return value of this macro should be something unexpected or unique that the user can handle properly in 
 * their code.
 * @param m_cond The given condition to assess
 * @param m_retval The return value for the current function
 * @returns Whatever value `m_retval` is set to
 */
#define ERR_FAIL_COND_R(m_cond, retval)                                                                                             \
    if (cond(m_cond)) {                                                                                                             \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" was true. Returning " _STR(retval) ".");   \
        return retval;                                                                                                              \
    }                                                                                                                               \

/**
 * @brief Throws an error whenever the given condition is true and returns a given value, as well as sending an error message to the user to understand what went 
 * wrong. Should be used in situations when you have both a return value and extra details for a user to handle. Generally speaking, the return value of this 
 * macro should be something unexpected or unique that the user can handle properly in their code.
 * @param m_cond The given condition to assess
 * @param m_msg The given error message to send to the console when the given condition is true
 * @param m_retval The return value for the current function
 * @returns Whatever value `m_retval` is set to
 */
#define ERR_FAIL_COND_MSG_R(m_cond, m_msg, retval)                                                                                          \
    if (cond(m_cond)) {                                                                                                                     \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" was true. Returning " _STR(retval) ".", m_msg);    \
        return retval;                                                                                                                      \
    }                                                                                                                                       \

/**
 * Error messages that report when an item is null
 */


/**
 * @brief Throws an error whenever the given condition is null. Should only be used in situations where there is no suitable return value for a function nor a 
 * suitable message to output into the console for the user.
 * @param m_item The given item to assess as null
 */
#define ERR_COND_NULL(m_item)                                                                       \
    if (cond(m_item == nullptr)) {                                                                  \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Item \"" _STR(m_item) "\" is null.");     \
        return;                                                                                     \
    }                                                                                               \

/**
 * @brief Throws an error message whenever the given condition is null, as well as sending an extra error message to the user to understand what went wrong. 
 * Should be used in situations when there is no suitable return value for a function. 
 * @param m_item The given item to assess as null
 * @param m_msg The given error message to send to the console when the given item is null
 */
#define ERR_COND_NULL_MSG(m_item, m_msg)                                                                \
    if (cond(m_item == nullptr)) {                                                                      \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Item \"" _STR(m_item) "\" is null.", m_msg);   \
        return;                                                                                         \
    }                                                                                                   \

/**
 * @brief Throws an error message whenever the given condition is null and returns a given value. Should only be used in situations when there is no suitable 
 * return value for a function. Generally speaking, the return value of this macro should be something unexpected or unique that the user can handle properly in 
 * their code.
 * @param m_item The given item to assess as null
 * @param m_retval The return value for the current function
 * @returns Whatever value `m_retval` is set to
 */
#define ERR_COND_NULL_R(m_item, retval)                                                                                         \
    if (cond(m_item == nullptr)) {                                                                                              \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Item \"" _STR(m_item) "\" is null. Returning " _STR(retval) ".");     \
        return retval;                                                                                                          \
    }                                                                                                                           \

/**
 * @brief Throws an error message whenever the given condition is null and returns a given value, as well as sending an extra error message to the user to 
 * understand what went wrong. Should be used in situations when you have both a return value and extra details for a user to handle. Generally speaking, the 
 * return value of this macro should be something unexpected or unique that the user can handle properly in their code.
 * @param m_item The given item to assess as null
 * @param m_msg The given error message to send to the console when the given item is null
 * @param m_retval The return value for the current function
 * @returns Whatever value `m_retval` is set to
 */
#define ERR_COND_NULL_MSG_R(m_item, m_msg, retval)                                                                                      \
    if (cond(m_item == nullptr)) {                                                                                                      \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Item \"" _STR(m_item) "\" is null. Returning " _STR(retval) ".", m_msg);      \
        return retval;                                                                                                                  \
    }                                                                                                                                   \

/**
 * @brief Throws an error whenever the given condition is true. This macro does not return early, but instead calls to `DEBUG_BREAK()` to add a breakpoint at the 
 * current function. Should only ever be used in a situation where the error the program currently faces cannot allow for the application to continue, and needs 
 * to be inspected carefully. 
 * @param m_cond The given condition to assess
 */
#define ERR_COND_FATAL(m_cond)                                                                                                      \
    if (cond(m_cond)) {                                                                                                             \
        _err_print_err(__FILE__, __FUNCTION__, __LINE__, "Condition \"" _STR(m_cond) "\" is true, a fatal error has occured.");     \
        DEBUG_BREAK();                                                                                                              \
    }                                                                                                                               \

