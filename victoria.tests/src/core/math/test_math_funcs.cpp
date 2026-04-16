#include "core/math/test_math_funcs.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/math/math_funcs.h>

static bool test_equality() {
	TEST_EQ(Math::is_equal(0.000000005, 0), true);
	TEST_EQ(Math::is_zero(0.00000014159), true);
	TEST_EQ(Math::is_equal(Math::PI / 2, (Math::PI * 2) / 4), true);
	return true;
}

static bool test_lerp() {
	double a = 0;
	double b = 10;
	TEST_EQ(Math::lerp(a, b, 0.5), 5);
	TEST_EQ(Math::lerp(a, b, 0.9), 9);

	TEST_EQ(Math::lerp(0.5, 1, 0.8), 0.9);
	TEST_EQ(Math::lerp(1, -4, 0.2), 0);

	TEST_EQ(Math::lerp(Math::PI, 2 * Math::PI, 0.5), (Math::PI * 3) / 2);
	return true;
}

void math_funcs_register_tests() {
	register_test(test_equality, "Math functions testing approximate equality of doubles");
	register_test(test_lerp, "Math functions testing linear interpolation of values");
}
