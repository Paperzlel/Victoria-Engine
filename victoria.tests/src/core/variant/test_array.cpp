#include "core/variant/test_array.h"

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

static bool array_test_strings_atomic() {
	Array a;
	a.push_back("Hello, World!");
	a.push_back("This is a mix of strings and other values!");
	a.push_back(12);
	a.push_back("soufhre gi rehb");
	a.push_back(Math::PI);
	a.push_back("\'\'\'\'\'");
	a.push_back(false);
	TEST_EQ(a[0], "Hello, World!");
	TEST_EQ(a[0].get_type(), Variant::STRING);
	TEST_EQ(a[2].get_type(), Variant::INT);
	TEST_EQ(a[6], Variant(false));
	Variant first = a.pop_front();
	TEST_EQ(first.get_type(), Variant::STRING);
	TEST_EQ(a.size(), 6);
	first = a.pop_back();
	TEST_EQ(first.get_type(), Variant::BOOL);
	a.clear();
	TEST_EQ(a.size(), 0);
	return true;
}

void array_register_tests() {
	register_test(array_test_basic, "Array reading, writing and clearing with atomic datatypes");
	register_test(array_test_strings_atomic, "Array reading, writing and clearing with atomic datatypes and strings.");
}
