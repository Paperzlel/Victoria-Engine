#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

extern VCORE_API DWORD CrashHandlerException(EXCEPTION_POINTERS *ep);

class CrashHandlerWindows {
public:
	static void initialize();
};
#endif // PLATFORM_WINDOWS