#pragma once

#include "math_funcs.h"

#include "core/typedefs.h"

class String;
struct Vector4i;

/**
 * @brief Structure that represents a 4-element vector, which is used for things such as colour values, Quaternions,
 * and so on.
 */
struct VAPI Vector4 {
	union {
		double elements[4] = {0};
		struct {
			double x;
			double y;
			double z;
			double w;
		};
	};

	FORCE_INLINE double &operator[](int p_index) {
		return elements[CLAMP(p_index, 0, 3)];
	}

	FORCE_INLINE const double &operator[](int p_index) const {
		return elements[CLAMP(p_index, 0, 3)];
	}

	FORCE_INLINE void operator=(const Vector4 &p_other) {
		x = p_other.x;
		y = p_other.y;
		z = p_other.z;
		w = p_other.w;
	}

	FORCE_INLINE Vector4 operator+(const Vector4 &p_other);
	FORCE_INLINE Vector4 &operator+=(const Vector4 &p_other);
	FORCE_INLINE Vector4 operator-(const Vector4 &p_other);
	FORCE_INLINE Vector4 &operator-=(const Vector4 &p_other);
	FORCE_INLINE Vector4 operator*(const Vector4 &p_other);
	FORCE_INLINE Vector4 operator*(const double p_scalar);
	FORCE_INLINE Vector4 &operator*=(const Vector4 &p_other);
	FORCE_INLINE Vector4 &operator*=(const double p_scalar);
	FORCE_INLINE Vector4 operator/(const Vector4 &p_other);
	FORCE_INLINE Vector4 operator/(const double p_scalar);
	FORCE_INLINE Vector4 &operator/=(const Vector4 &p_other);
	FORCE_INLINE Vector4 &operator/=(const double p_scalar);

	/**
	 * @brief Represents a vector with all components set to 0.
	 */
	static FORCE_INLINE Vector4 zero() {
		return Vector4(0, 0, 0, 0);
	}

	/**
	 * @brief Represents a vector with all components set to 1.
	 */
	static FORCE_INLINE Vector4 one() {
		return Vector4(1, 1, 1, 1);
	}

	FORCE_INLINE double length() const;
	FORCE_INLINE double length_squared() const;
	FORCE_INLINE double dot(const Vector4 &p_other) const;
	FORCE_INLINE Vector4 inverse() const;
	FORCE_INLINE void normalize();
	FORCE_INLINE Vector4 normalized() const;
	String stringify() const;

	FORCE_INLINE bool is_normalized() const;
	FORCE_INLINE bool is_equal(const Vector4 &p_other) const;

	operator String() const;
	operator Vector4i() const;

	Vector4() {}
	Vector4(const Vector4 &p_other) {
		*this = p_other;
	}
	Vector4(double p_x, double p_y, double p_z, double p_w) {
		x = p_x;
		y = p_y;
		z = p_z;
		w = p_w;
	}
};

Vector4 Vector4::operator+(const Vector4 &p_other) {
	Vector4 ret = *this;
	ret += p_other;
	return ret;
}

Vector4 &Vector4::operator+=(const Vector4 &p_other) {
	x += p_other.x;
	y += p_other.y;
	z += p_other.z;
	w += p_other.w;
	return *this;
}

Vector4 Vector4::operator-(const Vector4 &p_other) {
	Vector4 ret = *this;
	ret -= p_other;
	return ret;
}

Vector4 &Vector4::operator-=(const Vector4 &p_other) {
	x -= p_other.x;
	y -= p_other.y;
	z -= p_other.z;
	w -= p_other.w;
	return *this;
}

Vector4 Vector4::operator*(const Vector4 &p_other) {
	Vector4 ret = *this;
	ret *= p_other;
	return ret;
}

Vector4 Vector4::operator*(const double p_scalar) {
	Vector4 ret = *this;
	ret *= p_scalar;
	return ret;
}

Vector4 &Vector4::operator*=(const Vector4 &p_other) {
	x *= p_other.x;
	y *= p_other.y;
	z *= p_other.z;
	w *= p_other.w;
	return *this;
}

Vector4 &Vector4::operator*=(const double p_scalar) {
	x *= p_scalar;
	y *= p_scalar;
	z *= p_scalar;
	w *= p_scalar;
	return *this;
}

Vector4 Vector4::operator/(const Vector4 &p_other) {
	Vector4 ret = *this;
	ret /= p_other;
	return ret;
}

Vector4 Vector4::operator/(const double p_scalar) {
	Vector4 ret = *this;
	ret /= p_scalar;
	return ret;
}

Vector4 &Vector4::operator/=(const Vector4 &p_other) {
	x /= p_other.x;
	y /= p_other.y;
	z /= p_other.z;
	w /= p_other.w;
	return *this;
}

Vector4 &Vector4::operator/=(const double p_scalar) {
	x /= p_scalar;
	y /= p_scalar;
	z /= p_scalar;
	w /= p_scalar;
	return *this;
}

/**
 * @brief Obtains the magnitude of the current vector.
 */
double Vector4::length() const {
	return Math::sqrt(length_squared());
}

/**
 * @brief Obtains the squared magnitude of the current vector.
 */
double Vector4::length_squared() const {
	return x * x + y * y + z * z + w * w;
}

/**
 * @brief Finds the dot product of two vectors. This allows us to compare how similar one vector's direction is to
 * another - a value equal to the product of their lengths suggests an angle of 0, a value of zero suggest an angle of
 * PI / 2, and a value equal to the negative product of their lengths suggests an angle of PI, and so on. Angles are
 * clamped between -PI and PI, as the method looks for the smallest possible angle between the two.
 * @param p_other The other vector to compare against
 * @returns The dot product of the two vectors
 */
double Vector4::dot(const Vector4 &p_other) const {
	double ret = 0.0;
	ret += x * p_other.x;
	ret += y * p_other.y;
	ret += z * p_other.z;
	ret += w * p_other.w;
	return ret;
}

/**
 * @brief Obtains the inverse of the current vector
 */
Vector4 Vector4::inverse() const {
	return Vector4(-x, -y, -z, -w);
}

/**
 * @brief Normalizes the current vector, which is where one divides each component by the magnitude of the vector.
 */
void Vector4::normalize() {
	double len = length();

	if (len == 0) {
		return;
	}

	*this /= len;
}

/**
 * @brief Obtains a normalized version of the current vector. Does not normalize itself.
 */
Vector4 Vector4::normalized() const {
	Vector4 ret = *this;
	ret.normalize();
	return ret;
}

/**
 * @brief Checks if the current vector's magnitude is equal to 1.
 */
bool Vector4::is_normalized() const {
	return Math::is_equal(length(), 1);
}

/**
 * @brief Compares the current vector against another vector to see if they are approximately the same.
 */
bool Vector4::is_equal(const Vector4 &p_other) const {
	return Math::is_equal(x, p_other.x) && Math::is_equal(y, p_other.y) && Math::is_equal(z, p_other.z) &&
		   Math::is_equal(w, p_other.w);
}
