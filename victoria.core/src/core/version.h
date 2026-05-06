#pragma once

#undef _STR
#undef _MKSTR

// Use for parameter to string literals.
#define _STR(m_x) #m_x
// Use for macro expansion into string literals
#define _MKSTR(m_x) _STR(m_x)

// Bump the version number each update

#define VICTORIA_VERSION_NUMBER_MAJOR 0
#define VICTORIA_VERSION_NUMBER_MINOR 2
#define VICTORIA_VERSION_NUMBER_PATCH 0

/* clang-format off */
#define VICTORIA_VERSION_TYPE _STR(pre-alpha)
/* clang-format on */

#if defined(_WIN32) || defined(_WIN32) || defined(__WIN32__)
#	define VICTORIA_BUILD_OS _STR(windows)
#elif defined(__linux__) || defined(__gnu_linux__)
#	define VICTORIA_BUILD_OS _STR(linux)
#else
#	define VICTORIA_BUILD_OS _STR(unknown)
#endif

#define VICTORIA_VERSION_STRING                                                                                       \
	_MKSTR(VICTORIA_VERSION_NUMBER_MAJOR)                                                                             \
	"." _MKSTR(VICTORIA_VERSION_NUMBER_MINOR) "." _MKSTR(VICTORIA_VERSION_NUMBER_PATCH)

#define VICTORIA_FULL_VERSION_STRING VICTORIA_VERSION_STRING "_" VICTORIA_VERSION_TYPE "_" VICTORIA_BUILD_OS