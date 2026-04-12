#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#	include "core/data/hashtable.h"
#	include "core/string/vstring.h"
#	include "core/variant/variant.h"

HashTable<String, Variant> detect_wgl_version();

#endif // PLATFORM_WINDOWS