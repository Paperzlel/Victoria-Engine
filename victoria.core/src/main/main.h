#pragma once

#include "core/typedefs.h"

class VAPI Main {
public:
	static void print_help();

	static void set_should_quit(bool p_value);
	static bool get_should_quit();

	static Error setup(int argc, char *argv[]);
	static void finalize();
};
