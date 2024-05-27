#pragma once
// This means we only include this data once per export, since files like this will be used a bazillion times


/* Defines header file. These are used across the engine in place of standard variables
 * in order to have better memory precision.
 * A lot of this I took from Travis Vroman's Kohi series on YouTube, check him out :)
 */


/* Unsigned integers */

/* Unsigned 8-bit integer */
typedef unsigned char u8;

/* Unsigned 16-bit integer */
typedef unsigned short u16;

/* Unsigned 32-bit integer */
typedef unsigned int u32;

/* Unsigned 64-bit integer */
typedef unsigned long long u64;


/* Signed integers */

/* Signed 8-bit integer */
typedef signed char i8;

/* Sigend 16-bit integer */
typedef signed short i16;

/* Signed 32-bit integer */
typedef signed int i32;

/* Signed 64-bit integer */
typedef signed long long i64;


/* Floats */

/* 32-bit float */
typedef float f32;

/* 64-bit float */
typedef double f64;


/* Boolean types */

/* 32-bit boolean */
typedef int b32;

/* 8-bit boolean */
typedef char b8;

/* Define true and false */

#define TRUE 1
#define FALSE 0


/* String variable. Used instead of const char* because I prefer this*/
typedef const char* string;


#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

// Ensure all types are of the correct size, this is so that we don't pass incorrect values when using them.
STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

// Platform detection (for now only Windows)
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) 
#define PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif

// Export variables. Any API we want to expose uses this VAPI and is available from the .dll or .lib file.
#ifdef VEXPORT
#define VAPI __declspec(dllexport)
#else
#define VAPI __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define VAPI __declspec(dllimport)
#else
#define VAPI
#endif
#endif