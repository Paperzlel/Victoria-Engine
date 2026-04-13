#include "core/math/test_quaternion.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/math/quaternion.h>

static bool quaternion_test_basic() {
	Quaternion q;
	TEST_EQ(q.get_angle(), Math::PI);
	TEST_EQ(q.get_axis(), Vector3(0, 0, 0));
	TEST_EQ(q.is_identity(), false);

	return true;
}

static bool quaternion_test_rotation() {
	Quaternion q;
	q.from_axis_angle(Vector3(0, 1, 0), Math::DEG_TO_RAD_MULTIPLIER * 90);
	TEST_EQ(q.get_angle(), 90 * Math::DEG_TO_RAD_MULTIPLIER);
	TEST_EQ(q.get_axis(), Vector3(0, 1, 0));

	return true;
}

void quaternion_register_tests() {
	register_test(quaternion_test_basic, "Quaternion basic creation and attributes");
	register_test(quaternion_test_rotation, "Quaternion setting rotations from an axis-angle combination");
}
