#pragma once

#include "core/typedefs.h"
#include "core/error/error_macros.h"

class Main {
public:

    static void print_help();

    static void set_should_quit(bool p_value);

    static Error setup(int argc, char *argv[]);
    static Error start();
    static bool iteration();
    static void finalize();

    static void tests_entrypoint(int argc, char *argv[], bool &p_run_tests);
};
