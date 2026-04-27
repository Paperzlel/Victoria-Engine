#include "test_manager.h"

#include "test_macros.h"

#include "core/data/test_hashtable.h"
#include "core/data/test_list.h"
#include "core/data/test_vector.h"
#include "core/data/vector.h"
#include "core/math/test_mat4.h"
#include "core/math/test_quaternion.h"
#include "core/variant/test_array.h"
#include "core/variant/test_variant.h"

struct Test {
	PFN_test func = nullptr;
	const char *desc;

	Test() {}
	Test(PFN_test p_func, const char *p_desc) {
		func = p_func;
		desc = p_desc;
	}
};

// Put faith in our ability to write code

static Vector<Test> tests;

void register_test(PFN_test p_test, const char *p_desc) {
	Test t(p_test, p_desc);
	tests.push_back(t);
}

/**
 * @brief Calls every `register_test` function to properly append the tests and allow for them to be called when we run
 * the tests.
 */
void register_all_tests() {
	vector_register_tests();
	list_register_tests();
	hashtable_register_tests();

	mat4_register_tests();
	quaternion_register_tests();

	variant_register_tests();
	array_register_tests();
}

/**
 * @brief Runs every registered test, in order, from first to last. Order is preserved from the order they are called
 * in.
 */
void run_all_tests() {
	int passed = 0;
	int failed = 0;

	for (int i = 0; i < tests.size(); i++) {
		bool result = tests[i].func();

		if (result) {
			passed++;
		} else {
			MESSAGE("FAILED: %s", tests[i].desc);
			failed++;
		}
	}

	MESSAGE("All tests have now ran.\nPASSED: %i\nFAILED: %i", passed, failed);
}

int main(void) {
	// Needs stdout for pretty-printing.
	(void)OS::create();

	register_all_tests();
	run_all_tests();
	OS::destroy();
	return 0;
}