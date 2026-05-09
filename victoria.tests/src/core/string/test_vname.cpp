#include "core/string/test_vname.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/string/vname.h>

static bool vname_test_init_empty() {
	VName s;
	TEST_EQ(s.length(), 0);
	TEST_EQ(s.get_data(), nullptr);
	return true;
}

static bool vname_test_init_char_ptr() {
	VName s = "Hello, World!";
	TEST_EQ(s.length(), 13);
	TEST_NEQ(s.get_data(), nullptr);
	const char *test2 = "hello!";
	VName s2(test2);
	TEST_EQ(s2.length(), 6);
	TEST_NEQ(s2.get_data(), nullptr);
	return true;
}

static bool vname_test_init_string() {
	String s1 = "pwejf onb";
	VName s(s1);
	TEST_EQ(s.length(), s1.length());
	TEST_EQ(s.get_data(), s1.get_data());
	return true;
}

static bool vname_test_init_copy_from() {
	VName s = "Copying...";
	TEST_EQ(s.length(), 10);
	VName s2(s);
	TEST_EQ(s.length(), s2.length());
	TEST_EQ(s.get_data(), s2.get_data());
	return true;
}

static bool vname_test_init_move() {
	VName s = "Moving...";
	TEST_EQ(s.length(), 9);
	VName s2 = std::move(s);
	TEST_EQ(s2.length(), 9);
	TEST_EQ(s.length(), 0);
	TEST_EQ(s.get_data(), nullptr);
	return true;
}

static bool vname_test_assignment() {
	VName s = "Assigning A to B...";
	VName s2 = "Skipping C and D...";
	VName s3;
	TEST_NEQ(s.get_data(), s2.get_data());
	s = s2;
	TEST_EQ(s.get_data(), s2.get_data());
	TEST_EQ(s.length(), s2.length());
	s3 = "good grief... They forgot a test!";
	TEST_EQ(s3.length(), 33);
	s3 = std::move(s2);
	TEST_EQ(s3.length(), s.length());
	TEST_EQ(s2.length(), 0);
	TEST_EQ(s2.get_data(), 0);
	return true;
}

static bool vname_test_comparator() {
	const char *s1 = "Comparison is the thief of joy!";
	String s2 = s1;
	VName s3 = s1;
	VName s4 = s1;
	VName s5 = "Except now... Please work!";
	TEST_EQ(s1, s3);
	TEST_EQ(s2, s3);
	TEST_EQ(s3, s4);
	TEST_NEQ(s3, s5);
	return true;
}

static bool vname_test_get() {
	String rs = "Getting user settings...";
	VName s = rs;
	TEST_EQ(s.get_string(), rs);
	s = "Oops!";
	TEST_EQ(s[3], 's');
	return true;
}

void vname_register_tests() {
	register_test(vname_test_init_empty, "VName construction with no parameters");
	register_test(vname_test_init_char_ptr, "VName construction from string literals and char *");
	register_test(vname_test_init_string, "VName construction from our custom String type");
	register_test(vname_test_init_copy_from, "VName construction by copying from another VName");
	register_test(vname_test_init_move, "VName construction with the std::move operator");
	register_test(vname_test_assignment, "VName assignment from own types and other valid objects");
	register_test(vname_test_comparator, "VName comparisons with valid other types");
	register_test(vname_test_get, "VName getting functions for whole string and single characters");
}
