#include "core/variant/test_variant.h"
#include "test_macros.h"
#include "test_manager.h"

#include <core/variant/variant.h>

static bool variant_test_atomic() {
	// Basic C++ types (int, bool, double) require explicit casting for some reason
	Variant v = true;
	TEST_EQ(v.get_type(), Variant::BOOL);
	TEST_EQ(v, Variant(true));
	v = 12;
	TEST_EQ(v.get_type(), Variant::INT);
	TEST_EQ(v, Variant(12));
	v = Math::PI;
	TEST_EQ(v.get_type(), Variant::FLOAT);
	TEST_EQ(v, Variant(Math::PI));
	return true;
}

static bool variant_test_strings() {
	Variant v = "Hello, World!";
	TEST_EQ(v.get_type(), Variant::STRING);
	TEST_EQ(v, "Hello, World!");
	String refed = "TEST STRING WEEWOO!!";
	v = refed;
	TEST_EQ(v.get_type(), Variant::STRING);
	TEST_EQ(v, refed);
	refed = "strings should be different";
	TEST_NEQ(v, refed);
	return true;
}

void variant_register_tests() {
	register_test(variant_test_atomic, "Variant creation and casting from atomic datatypes.");
	register_test(variant_test_strings, "Variant creation and casting from string datatypes.");
}