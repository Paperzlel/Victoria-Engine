#include "core/math/test_mat4.h"

#include "test_macros.h"
#include "test_manager.h"

#include <core/math/mat4.h>

static bool mat4_test_operators() {
	Mat4 m = Mat4::identity();
	TEST_EQ(m[0], 1);
	TEST_EQ(m[5], 1);
	TEST_EQ(m[10], 1);
	TEST_EQ(m[15], 1);

	m += m;
	TEST_EQ(m[0], 2);
	TEST_EQ(m[5], 2);
	TEST_EQ(m[10], 2);
	TEST_EQ(m[15], 2);

	m *= Mat4();
	TEST_EQ(m[0], 2);
	TEST_EQ(m[5], 2);
	TEST_EQ(m[10], 2);
	TEST_EQ(m[15], 2);

	return true;
}

static bool mat4_test_modifiers() {
	Mat4 m;
	double det = m.determinant();
	TEST_EQ(det, 1);

	m = {1, 2, 3, 4, 0, 1, -1, 3, 2, 4, 1, -3, -2, -3, 1, 1};
	m.transpose();
	TEST_EQ(m[1], 0);
	TEST_EQ(m[2], 2);
	TEST_EQ(m[3], -2);
	TEST_EQ(m[4], 2);
	TEST_EQ(m[6], 4);
	TEST_EQ(m[7], -3);
	TEST_EQ(m[8], 3);
	TEST_EQ(m[9], -1);
	TEST_EQ(m[11], 1);
	TEST_EQ(m[12], 4);
	TEST_EQ(m[13], 3);
	TEST_EQ(m[14], -3);

	// NOTE: Inverting matrices is not yet tested, due to the current implementation being the inverse of a 3x3 and not
	// of a 4x4 (which should be doable,
	//  but currently is not needed as of current.)
	return true;
}

void mat4_register_tests() {
	register_test(mat4_test_operators, "4x4 matrix operations");
	register_test(mat4_test_modifiers, "4x4 matrix modifications");
}
