#pragma once

#include "vector2.h"

#include "core/os/memory.h"
#include "core/typedefs.h"

struct Mat2 {
public:
	double data[4] = {0};

	FORCE_INLINE double operator[](int p_index) {
		return data[CLAMP(p_index, 0, 3)];
	}

	FORCE_INLINE const double operator[](int p_index) const {
		return data[CLAMP(p_index, 0, 3)];
	}

	FORCE_INLINE void operator=(const Mat2 &p_other) {
		Memory::vcopy_memory(data, (void *)p_other.data, sizeof(double) * 4);
	}

	FORCE_INLINE Mat2 operator*(const Mat2 &p_other);
	FORCE_INLINE void operator*=(const Mat2 &p_other);
	FORCE_INLINE Vector2 operator*(const Vector2 &p_other);

	FORCE_INLINE double get_rotation() const;
	FORCE_INLINE void set_rotation(double p_rotation);

	FORCE_INLINE Vector2 get_scale() const;
	FORCE_INLINE void set_scale(const Vector2 &p_scale);

	FORCE_INLINE void rotate(double p_angle);
	FORCE_INLINE Mat2 rotated(double p_angle);
	FORCE_INLINE void scale(const Vector2 &p_scale);
	FORCE_INLINE Mat2 scaled(const Vector2 &p_scale);

	FORCE_INLINE double determinant() const;
	FORCE_INLINE void transpose();
	FORCE_INLINE Mat2 transposed();
	FORCE_INLINE void invert();
	FORCE_INLINE Mat2 inverse();

	Mat2() {
		data[0] = 1;
		data[3] = 1;
	}

	Mat2(const Mat2 &p_from) {
		*this = p_from;
	}

	Mat2(double p_rotation) {
		data[0] = 1;
		data[3] = 1;
		rotate(p_rotation);
	}

	Mat2(double a0, double a1, double a2, double a3) {
		data[0] = a0;
		data[1] = a1;
		data[2] = a2;
		data[3] = a3;
	}
};

Mat2 Mat2::operator*(const Mat2 &p_other) {
	Mat2 ret = *this;
	ret *= p_other;
	return ret;
}

void Mat2::operator*=(const Mat2 &p_other) {
	Mat2 tmp = *this;
	data[0] = tmp[0] * p_other[0] + tmp[1] * p_other[2];
	data[1] = tmp[0] * p_other[1] + tmp[1] * p_other[3];
	data[2] = tmp[2] * p_other[0] + tmp[3] * p_other[2];
	data[3] = tmp[2] * p_other[1] + tmp[3] * p_other[3];
}

Vector2 Mat2::operator*(const Vector2 &p_other) {
	Vector2 ret;
	ret.x = data[0] * p_other.x + data[1] * p_other.y;
	ret.y = data[2] * p_other.x + data[3] * p_other.y;
	return ret;
}

double Mat2::get_rotation() const {
	return Math::atan2(data[0], data[2]); // tan = sin / cos
}

void Mat2::set_rotation(double p_rotation) {
	Vector2 scale = get_scale();
	double sin = Math::sin(p_rotation);
	double cos = Math::cos(p_rotation);
	data[0] = cos;
	data[1] = -sin;
	data[2] = sin;
	data[3] = cos;
	set_scale(scale);
}

Vector2 Mat2::get_scale() const {
	double sign = determinant() > 0 ? 1 : -1;
	return Vector2(Vector2(data[0], data[1]).length() * sign, Vector2(data[2], data[3]).length() * sign);
}

void Mat2::set_scale(const Vector2 &p_scale) {
	double row1 = Vector2(data[0], data[1]).length();
	double row2 = Vector2(data[2], data[3]).length();
	data[0] /= row1;
	data[1] /= row1;
	data[2] /= row2;
	data[3] /= row2;

	data[0] *= p_scale.x;
	data[1] *= p_scale.y;
	data[2] *= p_scale.x;
	data[3] *= p_scale.y;
}

void Mat2::rotate(double p_angle) {
	*this = rotated(p_angle);
}

Mat2 Mat2::rotated(double p_angle) {
	Mat2 ret = *this;
	double sin = Math::sin(p_angle);
	double cos = Math::cos(p_angle);

	Mat2 tmp(cos, sin, -sin, cos);
	return ret * tmp;
}

void Mat2::scale(const Vector2 &p_scale) {
	*this = scaled(p_scale);
}

Mat2 Mat2::scaled(const Vector2 &p_scale) {
	Mat2 tmp(p_scale.x, 0, 0, p_scale.y);
	return *this * tmp;
}

double Mat2::determinant() const {
	return (data[0] * data[3]) - (data[1] * data[2]);
}

void Mat2::transpose() {
	double tmp = data[1];
	data[1] = data[2];
	data[2] = tmp;
}

Mat2 Mat2::transposed() {
	Mat2 ret = *this;
	ret.transpose();
	return ret;
}

void Mat2::invert() {
	double det = determinant();

	if (Math::is_zero(det)) {
		return; // Is a singular matrix
	}

	det = 1 / det;

	double a0 = data[0] * det;
	data[0] = data[3] * det;
	data[3] = a0;
	data[1] = -data[1] * det;
	data[2] = -data[2] * det;
}

Mat2 Mat2::inverse() {
	Mat2 ret = *this;
	ret.invert();
	return ret;
}
