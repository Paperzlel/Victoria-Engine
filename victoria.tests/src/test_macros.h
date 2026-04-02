#pragma once

#include <core/os/os.h> // IWYU pragma: keep

#define MESSAGE(m_message, ...)                                                                                       \
	if (OS::get_singleton()) {                                                                                        \
		OS::get_singleton()->print(m_message, __VA_ARGS__);                                                           \
	}

#define MESSAGE_FAILURE(m_message)                                                                                    \
	if (OS::get_singleton()) {                                                                                        \
		OS::get_singleton()->print_error(__FILE__, FUNCTION_STR, __LINE__, m_message, "", ERROR_ERR);                 \
	}

#define TEST_EQ(m_value, m_expected)                                                                                  \
	if (m_value != m_expected) {                                                                                      \
		MESSAGE_FAILURE("Expected " _STR(m_value) " to be equal to " _STR(m_expected) ".");                           \
		return false;                                                                                                 \
	}

#define TEST_NEQ(m_value, m_expected)                                                                                 \
	if (m_value == m_expected) {                                                                                      \
		MESSAGE_FAILURE("Expected " _STR(m_value) " to not be equal to " _STR(m_expected) ".");                       \
		return false;                                                                                                 \
	}

