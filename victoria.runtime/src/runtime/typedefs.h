#pragma once

#include <core/typedefs.h>

// Export/import flags for the runtime API
#if VRUNTIME_API_EXPORT
#	if defined(_MSC_VER)
#		define VRUNTIME_API __declspec(dllexport)
#	elif defined(__GNUC__) || defined(__clang__)
#		define VRUNTIME_API __attribute__((visibility("default")))
#	else
#		define VRUNTIME_API
#	endif
#else
#	if defined(_MSC_VER)
#		define VRUNTIME_API __declspec(dllimport)
#	elif defined(__GNUC__) || defined(__clang__)
#		define VRUNTIME_API
#	else
#		define VRVRUNTIME_API
#	endif
#endif
