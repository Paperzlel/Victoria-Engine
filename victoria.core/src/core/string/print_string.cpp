#include "core/string/print_string.h"

#include "core/os/os.h"
#include "core/variant/variant.h"

String stringify_variant(const Variant &p_var) {
	return p_var.operator String();
}

bool is_printing_verbose() {
	if (OS::get_singleton()) {
		return OS::get_singleton()->is_stdout_verbose();
	}

	return false;
}

void _print_line(const String &s) {
	if (OS::get_singleton()) {
		OS::get_singleton()->print(s.get_data());
	}
}

void _print_error(const String &s) {
	if (OS::get_singleton()) {
		OS::get_singleton()->printerr(s);
	}
}