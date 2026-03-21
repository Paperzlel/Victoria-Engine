#pragma once

#undef _STR

#define _STR(m_x) #m_x

// Bump the version number each update

#define VICTORIA_VERSION_NUMBER_MAJOR 0
#define VICTORIA_VERSION_NUMBER_MINOR 2
#define VICTORIA_VERSION_NUMBER_PATCH 0

#define VICTORIA_VERSION_TYPE _STR(pre - alpha)

#if defined(_WIN32) || defined(_WIN32) || defined(__WIN32__)
#	define VICTORIA_BUILD_OS _STR(windows)
#elif defined(__linux__) || defined(__gnu_linux__)
#	define VICTORIA_BUILD_OS _STR(linux)
#else
#	define VICTORIA_BUILD_OS _STR(unknown)
#endif
