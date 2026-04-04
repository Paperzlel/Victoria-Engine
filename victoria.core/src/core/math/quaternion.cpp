#include "core/math/quaternion.h"

#include "core/math/mat4.h"

// Methods derived from https://danceswithcode.net/engineeringnotes/quaternions/quaternions.html

/**
 * @brief Checks if the given quaternion is an identity quaternion, which is when all the axes are set to zero and the
 * angle is set to 1.
 * @returns `true` if the quaternion is indeed an identity, and `false` if not.
 */
bool Quaternion::is_identity() const {
	return length() == 1 && x == 0 && y == 0 && z == 0;
}

/**
 * @brief Sets the current quaternion to be equal to an axis-angle representation of a rotation in quaternion form.
 * @param p_axis The given axis to rotate about
 * @param p_angle The angle to rotate around the given axis by, in radians
 */
void Quaternion::from_axis_angle(const Vector3 &p_axis, double p_angle) {
	// Assume vector is normalized.
	double half_sin = Math::sin(p_angle / 2);
	x = p_axis.x * half_sin;
	y = p_axis.y * half_sin;
	z = p_axis.z * half_sin;
	w = Math::cos(p_angle / 2);
}

/**
 * @brief Obtains the number of radians one wants to rotate about the current axis by.
 * @returns The number of radians to rotate by
 */
double Quaternion::get_angle() const {
	return 2 * Math::acos(w);
}

/**
 * @brief Obtains the axis that this quaternion is rotating about.
 * @returns The axis of rotation for the current quaternion
 */
Vector3 Quaternion::get_axis() const {
	double angle = get_angle();
	if (is_identity() || angle == 0) {
		return Vector3::zero(); // acos of 1 is zero, and sin of 0 is zero so we want to avoid a divide-by-zero error
								// here
	}
	double half_sin = Math::sin(angle / 2);
	return Vector3(x / half_sin, y / half_sin, z / half_sin);
}

/**
 * @brief Converts the current quaternion into a rotation matrix, which can then be used to construct a model matrix or
 * something else along those lines.
 * @returns The current quaternion as a rotation matrix
 */
Mat4 Quaternion::to_rotation_matrix() const {
	Mat4 ret = Mat4::identity();
	double qxs = 2 * x * x;
	double qys = 2 * y * y;
	double qzs = 2 * z * z;
	ret.data[0] = 1 - qys - qzs;
	ret.data[5] = 1 - qxs - qzs;
	ret.data[10] = 1 - qxs - qys;

	double qxqy = 2 * x * y;
	double qzqw = 2 * z * w;
	ret.data[1] = qxqy - qzqw;
	ret.data[4] = qxqy + qzqw;

	double qxqz = 2 * x * z;
	double qyqw = 2 * y * w;
	ret.data[2] = qxqz + qyqw;
	ret.data[8] = qxqz - qyqw;

	double qyqz = 2 * y * z;
	double qxqw = 2 * x * w;
	ret.data[6] = qyqz - qxqw;
	ret.data[9] = qyqz + qxqw;
	return ret;
}

/**
 * @brief Creates and returns a quaternion that was constructed from a set of Euler angles. Expects the rotational
 * order for said Euler angles to be YXZ.
 * @param p_x The x rotational factor, in radians
 * @param p_y The y rotational factor, in radians
 * @param p_z The z rotational factor, in radians
 * @returns A quaternion equivalent to the given Euler angles
 */
Quaternion Quaternion::from_euler_angles(double p_x, double p_y, double p_z) {
	Vector4 ret;
	double cu = Math::cos(p_y / 2);
	double cv = Math::cos(p_x / 2);
	double cw = Math::cos(p_z / 2);
	double su = Math::sin(p_y / 2);
	double sv = Math::sin(p_x / 2);
	double sw = Math::sin(p_z / 2);

	ret.x = su * sw * cv + cu * cv * sw;
	ret.y = su * cv * cw - sv * sw * cu;
	ret.z = sw * cu * cv - su * sv * cw;
	ret.w = su * sv * sw + cu * cv * cw;
	return Quaternion(ret);
}

/**
 * @brief Converts and obtains the current quaternion's Euler angles, by getting them from a rotation matrix.
 * @returns The rotation of the current quaternion in Euler angles
 */
Vector3 Quaternion::to_euler_angles() const {
	return to_rotation_matrix().get_euler_angles();
}

double Quaternion::length_squared() const {
	return x * x + y * y + z * z + w * w;
}

double Quaternion::length() const {
	return Math::sqrt(length_squared());
}

void Quaternion::invert() {
	x = -x;
	y = -y;
	z = -z;
}

Quaternion Quaternion::inverse() const {
	Quaternion ret = *this;
	ret.invert();
	return ret;
}

void Quaternion::operator*=(const Quaternion &p_other) {
	Quaternion q = *this;
	w = q.w * p_other.w - q.x * p_other.x - q.y * p_other.y - q.z * p_other.z;
	x = q.w * p_other.x + q.x * p_other.w - q.y * p_other.z + q.z * p_other.y;
	y = q.w * p_other.y + q.x * p_other.z + q.y * p_other.w - q.z * p_other.x;
	z = q.w * p_other.z - q.x * p_other.y + q.y * p_other.x + q.z * p_other.w;
}

Quaternion Quaternion::operator*(const Quaternion &p_other) {
	Quaternion ret = *this;
	ret *= p_other;
	return ret;
}
