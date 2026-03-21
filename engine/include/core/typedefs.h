#pragma once
// Header file for our commonly-used definitions

// IWYU pragma: always_keep

#include "core/error/error_types.h" // IWYU pragma: keep

#include <utility> // IWYU pragma: keep

typedef signed char i8;

typedef signed short i16;

typedef signed int i32;

typedef signed long long i64;

typedef unsigned char u8;

typedef unsigned short u16;

typedef unsigned int u32;

typedef unsigned long long u64;

static_assert(sizeof(i8) == 1, "Expected a signed 8-bit integer to have 1 byte.");
static_assert(sizeof(i16) == 2, "Expected a signed 16-bit integer to have 2 bytes.");
static_assert(sizeof(i32) == 4, "Expected a signed 32-bit integer to have 4 bytes.");
static_assert(sizeof(i64) == 8, "Expected a signed 64-bit integer to have 8 bytes.");
static_assert(sizeof(u8) == 1, "Expected an unsigned 8-bit integer to have 1 byte.");
static_assert(sizeof(u16) == 2, "Expected an unsigned 16-bit integer to have 2 bytes.");
static_assert(sizeof(u32) == 4, "Expected an unsigned 32-bit integer to have 4 bytes.");
static_assert(sizeof(u64) == 8, "Expected an unsigned 64-bit integer to have 8 bytes.");

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
FORCE_INLINE u32 next_po2(u32 p_input) {
	u32 res = p_input;
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
FORCE_INLINE u32 find_log2(u32 p_input) {
	u32 res = 0;
	while (p_input >>= 1) {
		res++;
	}

	return res;
}

// http://loungecpp.wikidot.com/tips-and-tricks:indices
// Expands a variadic macro into its constituent parts

template <u64... Is>
struct Indicies {};

template <u64 N, u64... Is>
struct BuildIndicies : BuildIndicies<N - 1, N - 1, Is...> {};

template <u64... Is>
struct BuildIndicies<0, Is...> : Indicies<Is...> {};