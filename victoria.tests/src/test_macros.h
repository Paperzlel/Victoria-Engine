#pragma once

#include <core/os/os.h> // IWYU pragma: keep

/**
 * @brief Prints a (formatted) message to the console.
 * @param m_message The message to output.
 * @param ... Variadic message arguments to format into the message.
 */
#define MESSAGE(m_message, ...)                                                                                       \
	if (OS::get_singleton()) {                                                                                        \
		OS::get_singleton()->print(m_message, __VA_ARGS__);                                                           \
	}

/**
 * @brief Prints a (formatted) error messafe to the console.
 * @param m_message The error message to output.
 * @param ... Variadic message arguments to format into the message.
 */
#define MESSAGE_FAILURE(m_message)                                                                                    \
	if (OS::get_singleton()) {                                                                                        \
		OS::get_singleton()->print_error(__FILE__, FUNCTION_STR, __LINE__, m_message, "", ERROR_ERR);                 \
	}

/**
 * @brief Checks to see if the given values are equal in their output. If not, prints an error message and returns
 * false. Only to be used in test functions.
 * @param m_value The left-hand value to check.
 * @param m_expected The right-hand value to check.
 */
#define TEST_EQ(m_value, m_expected)                                                                                  \
	if (unlikely(m_value != m_expected)) {                                                                            \
		MESSAGE_FAILURE("Expected " _STR(m_value) " to be equal to " _STR(m_expected) ".");                           \
		return false;                                                                                                 \
	}

/**
 * @brief Checks to see if the given value was not equal to the output. If not, prints and error message and returns
 * false. Only to be used in test functions.
 * @param m_value The left-hand value to check.
 * @param m_expected The right-hand value to check.
 */
#define TEST_NEQ(m_value, m_expected)                                                                                 \
	if (unlikely(m_value == m_expected)) {                                                                            \
		MESSAGE_FAILURE("Expected " _STR(m_value) " to not be equal to " _STR(m_expected) ".");                       \
		return false;                                                                                                 \
	}

