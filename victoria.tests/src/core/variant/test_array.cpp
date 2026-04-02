#include "test_array.h"
#include "test_macros.h"
#include "test_manager.h"

#include <core/variant/array.h>
#include <core/variant/variant.h>

static bool array_test_basic() {
	Array a;
	a.push_back(1);
	a.push_back(true);
	a.push_back(3.14159);
	TEST_EQ(int32_t(a[0]), 1);
	TEST_EQ(bool(a[1]), true);
	TEST_EQ(double(a[2]), 3.14159);

	a.clear();
	TEST_EQ(a.size(), 0);

	return true;
}

void array_register_tests() {
	register_test(array_test_basic, "Array reading, writing and clearing with atomic datatypes");
}
