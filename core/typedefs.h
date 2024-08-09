#ifndef TYPEDEFS_H
#define TYPEDEFS_H

typedef unsigned char uint8;

typedef unsigned short uint16;

typedef unsigned int uint32;

typedef unsigned long long uint64;

static_assert(sizeof(uint8) == 1, "Expected an unsigned 8-bit integer to have 1 byte.");
static_assert(sizeof(uint16) == 2, "Expected an unsigned 16-bit integer to have 2 bytes.");
static_assert(sizeof(uint32) == 4, "Expected an unsigned 32-bit integer to have 4 bytes.");
static_assert(sizeof(uint64) == 8, "Expected an unsigned 64-bit integer to have 8 bytes.");

#define CLAMP(value, min, max) (value <= min) ? min : (value >= max) ? max : value

#define _STR(v_x) #v_x 

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
#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#define NO_INLINE __declspec(noinline)
#else
#define FORCE_INLINE static inline
#define NO_INLINE
#endif

// Debug toggling
#ifdef DEBUG
#define DEBUG_ENABLED
#else
#define RELEASE_ENABLED
#endif

#endif