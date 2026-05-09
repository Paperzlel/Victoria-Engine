#pragma once

#include "core/typedefs.h"

// NOTE: These probably shouldn't be exposed, but they're needed for basic applications that use VName. Headless
// mode for core should be a feature we prioritise to fix this.

VAPI void register_core_types();

VAPI void unregister_core_types();
