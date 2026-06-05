#pragma once

#include "core/typedefs.h"

// NOTE: Exposed via `VName`, thanks to how __declspec(dllexport) works on Windows.

void register_core_types();

void unregister_core_types();
