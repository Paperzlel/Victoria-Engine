#pragma once
// Header file for our commonly-used definitions

// IWYU pragma: always_keep

#include "core/error/error_types.h" // IWYU pragma: keep

#include <cstdint>
#include <utility> // IWYU pragma: keep

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#	define PLATFORM_WINDOWS 1
#	ifndef _WIN64
#		error "64-bit Windows is required!"
#	endif
#elif defined(__linux__) || defined(__gnu_linux__)
#	define PLATFORM_LINUX 1
#else
#	error "Platform not detected!"
#endif

// Export/import functions
#if VEXPORT
#	if defined(__GNUC__) || defined(__clang__)
#		define VAPI __attribute__((visibility("default")))
#	elif defined(_MSC_VER)
#		define VAPI __declspec(dllexport)
#	else
#		define VAPI
#	endif
#else
#	if defined(__GNUC__) || defined(__clang__)
#		define VAPI
#	elif defined(_MSC_VER)
#		define VAPI __declspec(dllimport)
#	else
#		define VAPI
#	endif
#endif

// Inline variables
#ifndef ALWAYS_INLINE
#	if defined(__GNUC__)
#		define ALWAYS_INLINE __attribute__((always_inline)) inline
#	elif defined(_MSC_VER)
#		define ALWAYS_INLINE __forceinline
#	else
#		define ALWAYS_INLINE inline
#	endif
#endif

#ifndef FORCE_INLINE
#	ifdef DEBUG
#		define FORCE_INLINE inline
#	else
#		define FORCE_INLINE ALWAYS_INLINE
#	endif
#endif

// Define size_t
#if PLATFORM_LINUX
typedef __SIZE_TYPE__ size_t;
#endif

// Debug enabled/disabled
#ifdef DEBUG
#	define DEBUG_ENABLED
#else
#	define RELEASE_ENABLED
#endif

// Unit tests enabled/disabled
#ifdef TESTS
#	define TESTS_ENABLED
#endif

// Generic clamping function
#define CLAMP(value, min, max) (value <= min) ? min : (value >= max) ? max : value

// Generic variable to string literal conversion
#define _STR(v_x) #v_x

// Generic swap macro
#define SWAP(x, y) std::swap((x), (y))

/**
 * @brief Finds the next power of 2 for the input value, or returns the current value if the input is a power of 2.
 * @param p_input The value to find the next power of 2 for
 * @returns The next power of 2
 */
FORCE_INLINE uint32_t next_po2(uint32_t p_input) {
	uint32_t res = p_input;
	res--;
	res |= res >> 1;
	res |= res >> 2;
	res |= res >> 4;
	res |= res >> 8;
	res |= res >> 16;
	res++;
	return res;
}

/**
 * @brief Finds the current power of 2 for the input value. Acts as an integer version for log2(n) and rounds down
 * @param p_input The input value to look for
 * @returns The nearest power of 2
 */
FORCE_INLINE uint32_t find_log2(uint32_t p_input) {
	uint32_t res = 0;
	while (p_input >>= 1) {
		res++;
	}

	return res;
}

// http://loungecpp.wikidot.com/tips-and-tricks:indices
// Expands a variadic macro into its constituent parts

template <uint64_t... Is>
struct Indicies {};

template <uint64_t N, uint64_t... Is>
struct BuildIndicies : BuildIndicies<N - 1, N - 1, Is...> {};

template <uint64_t... Is>
struct BuildIndicies<0, Is...> : Indicies<Is...> {};