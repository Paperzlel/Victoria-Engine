#pragma once

#include "core/error/error_types.h"

typedef signed char i8;

typedef signed short i16;

typedef signed int i32;

typedef signed long long i64;


typedef unsigned char u8;

typedef unsigned short u16;

typedef unsigned int u32;

typedef unsigned long long u64;

typedef float f32;

typedef double f64;

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
#define PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit Windows is required!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
#define PLATFORM_LINUX 1
#else
#error "Platform is not compatible!"
#endif

// Inline variables
#ifndef DEBUG
#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#define NO_INLINE __declspec(noinline)
#else
#define FORCE_INLINE __attribute__((always_inline)) inline
#define NO_INLINE
#endif
#else
#define FORCE_INLINE inline
#define NO_INLINE
#endif

#if PLATFORM_LINUX
typedef __SIZE_TYPE__ size_t;
#endif

// Debug toggling
#ifdef DEBUG
#define DEBUG_ENABLED
#else
#define RELEASE_ENABLED
#endif

#ifdef TESTS
#define TESTS_ENABLED
#endif

#define CLAMP(value, min, max) (value <= min) ? min : (value >= max) ? max : value

#define _STR(v_x) #v_x 


// http://loungecpp.wikidot.com/tips-and-tricks:indices 
// Expands a variadic macro into its constituent parts

template<u64... Is>
struct Indicies {};

template<u64 N, u64... Is>
struct BuildIndicies : BuildIndicies<N - 1, N - 1, Is...> {};

template<u64... Is>
struct BuildIndicies<0, Is...> : Indicies<Is...> {};