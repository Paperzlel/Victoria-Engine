#pragma once

#include "core/typedefs.h"

VCORE_API Error core_initialize(int argc, char **argv);

VCORE_API void core_finalize();

VCORE_API void core_list_arguments();
