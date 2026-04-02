#include "test_manager.h"

#include <core/os/os.h>

int main(void) {
	// Don't need result and it produces an error.
	(void)OS::initialize_for_tests();

	register_all_tests();
	run_all_tests();
	return 0;
}
