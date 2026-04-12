#pragma once

#include "vector3.h"
#include "vector4.h"

#include "core/typedefs.h"

struct Mat4;

/**
 * @brief Struct that represents a quaternion, which is primarily used for axis-angle rotations in 3D space. In our
 * implementation, the w component of the quaternion represents the angle, as the x, y and z components are used for 3D
 * space, so bear that in mind when converting between the two.
 */
struct VAPI Quaternion {
	union {
		double elements[4] = {0};
		struct {
			double x;
			double y;
			double z;
			double w;
		};
	};

	FORCE_INLINE double operator[](int p_index) {
		return elements[p_index];
	}
	FORCE_INLINE const double operator[](int p_index) const {
		return elements[p_index];
	}

	FORCE_INLINE void operator=(const Quaternion &p_other) {
		x = p_other.x;
		y = p_other.y;
		z = p_other.z;
		w = p_other.w;
	}

	bool is_identity() const;

	void from_axis_angle(const Vector3 &p_axis, double p_angle);

	double get_angle() const;
	Vector3 get_axis() const;
	Mat4 to_rotation_matrix() const;
	static Quaternion from_euler_angles(double p_x, double p_y, double p_z);
	Vector3 to_euler_angles() const;

	double length_squared() const;
	double length() const;

	void invert();
	Quaternion inverse() const;

	void operator*=(const Quaternion &p_other);
	Quaternion operator*(const Quaternion &p_other);

	Quaternion() {}
	Quaternion(const Quaternion &p_other) {
		*this = p_other;
	}

	Quaternion(const Vector3 &p_axis, double p_angle) {
		from_axis_angle(p_axis, p_angle);
	}

	Quaternion(const Vector4 &p_from) {
		x = p_from.x;
		y = p_from.y;
		z = p_from.z;
		w = p_from.w;
	}
};
