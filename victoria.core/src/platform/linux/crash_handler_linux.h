#pragma once

#include "core/typedefs.h"
#if PLATFORM_LINUX

class CrashHandlerLinux {
public:
	void initialize();
};

#endif // PLATFORM_LINUX