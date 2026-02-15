#pragma once

#include <core/typedefs.h>
#if PLATFORM_WINDOWS

#include <core/data/hashtable.h>

HashTable<String, String> detect_wgl_version();

#endif //PLATFORM_WINDOWS