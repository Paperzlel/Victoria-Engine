#pragma once

#include "core/typedefs.h"

typedef bool (*PFN_test)();

void register_test(PFN_test p_test, const char *p_desc);

void register_all_tests();

void run_all_tests();
