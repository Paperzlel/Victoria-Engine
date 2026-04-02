#include "test_manager.h"

#include <core/os/os.h>

int main(void) {
	OS *os = OS::initialize_for_tests();

	register_all_tests();
	run_all_tests();
	return 0;
}
