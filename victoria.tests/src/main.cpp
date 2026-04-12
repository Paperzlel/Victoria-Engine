#include "test_manager.h"

#include <core/os/os.h>

int main(void) {
	// Only create the OS since it should create stdout for us as well, don't need the other core features.
	(void)OS::create();

	register_all_tests();
	run_all_tests();
	OS::destroy();
	return 0;
}
