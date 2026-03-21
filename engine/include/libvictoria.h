#pragma once

#include "core/config/victoria_instance.h"
#include "core/typedefs.h"

VAPI VictoriaInstance *victoria_create_instance(int argc, char *argv[]);

VAPI void victoria_destroy_instance(VictoriaInstance *p_ptr);
