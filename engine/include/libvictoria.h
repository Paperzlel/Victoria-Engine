#pragma once

#include "core/typedefs.h"
#include "core/config/victoria_instance.h"

VAPI VictoriaInstance *victoria_create_instance(int argc, char *argv[]);

VAPI void victoria_destroy_instance(VictoriaInstance *p_ptr);
