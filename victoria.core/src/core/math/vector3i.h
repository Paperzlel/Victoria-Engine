#pragma once

#include "math_funcs.h"

#include "core/typedefs.h"

class String;
struct Vector3;

/**
 * @brief Struct that represents a vector in 3D space.
 */
struct VAPI Vector3i {
public:
	union {
		int64_t elements[3] = {0};
		struct {
			int64_t x;
			int64_t y;
			int64_t z;
		};
	};

	FORCE_INLINE int64_t &operator[](int p_index) {
		return elements[CLAMP(p_index, 0, 2)];
	}

	FORCE_INLINE const int64_t &operator[](int p_index) const {
		return elements[CLAMP(p_index, 0, 2)];
	}

	FORCE_INLINE void operator=(const Vector3i &p_other) {
		x = p_other.x;
		y = p_other.y;
		z = p_other.z;
	}

	FORCE_INLINE Vector3i operator+(const Vector3i &p_other);
	FORCE_INLINE Vector3i &operator+=(const Vector3i &p_other);
	FORCE_INLINE Vector3i operator-(const Vector3i &p_other);
	FORCE_INLINE Vector3i &operator-=(const Vector3i &p_other);
	FORCE_INLINE Vector3i operator*(const Vector3i &p_other);
	FORCE_INLINE Vector3i operator*(const int64_t p_scalar);
	FORCE_INLINE Vector3i &operator*=(const Vector3i &p_other);
	FORCE_INLINE Vector3i &operator*=(const int64_t p_scalar);
	FORCE_INLINE Vector3i operator/(const Vector3i &p_other);
	FORCE_INLINE Vector3i operator/(const int64_t p_scalar);
	FORCE_INLINE Vector3i &operator/=(const Vector3i &p_other);
	FORCE_INLINE Vector3i &operator/=(const int64_t p_scalar);

	/**
	 * @brief Represents a vector with each component set to 0.
	 */
	static FORCE_INLINE Vector3i zero() {
		return Vector3i(0, 0, 0);
	}

	/**
	 * @brief Represents a vector with each component set to 1.
	 */
	static FORCE_INLINE Vector3i one() {
		return Vector3i(1, 1, 1);
	}

	/**
	 * @brief Represents a unit vector which points along the positive y-axis.
	 */
	static FORCE_INLINE Vector3i up() {
		return Vector3i(0, 1, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the negative y-axis.
	 */
	static FORCE_INLINE Vector3i down() {
		return Vector3i(0, -1, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the positive x-axis.
	 */
	static FORCE_INLINE Vector3i left() {
		return Vector3i(-1, 0, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the negative x-axis.
	 */
	static FORCE_INLINE Vector3i right() {
		return Vector3i(1, 0, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the negative z-axis (since we use a right-handed coordinate
	 * system, positive Z points into the screen)
	 */
	static FORCE_INLINE Vector3i forward() {
		return Vector3i(0, 0, -1);
	}

	/**
	 * @brief Represents a unit vector which points along the positive z-axis (since we use a right-handed coordinate
	 * system, positive Z points into the screen)
	 */
	static FORCE_INLINE Vector3i backward() {
		return Vector3i(0, 0, 1);
	}

	FORCE_INLINE double length() const;
	FORCE_INLINE int64_t length_squared() const;
	FORCE_INLINE int64_t angle(const Vector3i &p_other) const;
	FORCE_INLINE int64_t dot(const Vector3i &p_other) const;
	FORCE_INLINE Vector3i cross(const Vector3i &p_other) const;
	FORCE_INLINE Vector3i inverse() const;
	FORCE_INLINE void normalize();
	FORCE_INLINE Vector3i normalized() const;
	FORCE_INLINE Vector3i lerp(const Vector3i &p_to, double p_factor) const;
	String stringify() const;

	FORCE_INLINE bool is_normalized() const;
	FORCE_INLINE bool is_equal(const Vector3i &p_other) const;

	operator String() const;
	operator Vector3() const;

	Vector3i() {}
	FORCE_INLINE Vector3i(const Vector3i &p_other) {
		*this = p_other;
	}
	FORCE_INLINE Vector3i(int64_t p_x, int64_t p_y, int64_t p_z) {
		x = p_x;
		y = p_y;
		z = p_z;
	}
};

Vector3i Vector3i::operator+(const Vector3i &p_other) {
	Vector3i ret = *this;
	ret += p_other;
	return ret;
}

Vector3i &Vector3i::operator+=(const Vector3i &p_other) {
	x += p_other.x;
	y += p_other.y;
	z += p_other.z;
	return *this;
}

Vector3i Vector3i::operator-(const Vector3i &p_other) {
	Vector3i ret = *this;
	ret -= p_other;
	return ret;
}

Vector3i &Vector3i::operator-=(const Vector3i &p_other) {
	x -= p_other.x;
	y -= p_other.y;
	z -= p_other.z;
	return *this;
}

Vector3i Vector3i::operator*(const Vector3i &p_other) {
	Vector3i ret = *this;
	ret *= p_other;
	return ret;
}

Vector3i Vector3i::operator*(const int64_t p_scalar) {
	Vector3i ret = *this;
	ret *= p_scalar;
	return ret;
}

Vector3i &Vector3i::operator*=(const Vector3i &p_other) {
	x *= p_other.x;
	y *= p_other.y;
	z *= p_other.z;
	return *this;
}

Vector3i &Vector3i::operator*=(const int64_t p_scalar) {
	x *= p_scalar;
	y *= p_scalar;
	z *= p_scalar;
	return *this;
}

Vector3i Vector3i::operator/(const Vector3i &p_other) {
	Vector3i ret = *this;
	ret /= p_other;
	return ret;
}

Vector3i Vector3i::operator/(const int64_t p_scalar) {
	Vector3i ret = *this;
	ret /= p_scalar;
	return ret;
}

Vector3i &Vector3i::operator/=(const Vector3i &p_other) {
	x /= p_other.x;
	y /= p_other.y;
	z /= p_other.z;
	return *this;
}

Vector3i &Vector3i::operator/=(const int64_t p_scalar) {
	x /= p_scalar;
	y /= p_scalar;
	z /= p_scalar;
	return *this;
}

/**
 * @brief Gets the length of a given vector.
 */
double Vector3i::length() const {
	return Math::sqrt(length_squared());
}

/**
 * @brief Gets the square of the magnitude of a given vector.
 */
int64_t Vector3i::length_squared() const {
	return x * x + y * y + z * z;
}

/**
 * @brief Gets the smallest angle between two given vectors (between `-PI` and `PI`).
 * @param p_other The other vector to find the angle between
 */
int64_t Vector3i::angle(const Vector3i &p_other) const {
	int64_t lhs_len = length();
	int64_t rhs_len = p_other.length();

	// <= is not needed because imaginary numbers are not included in this application but god knows what the C++
	// compiler is capable of
	if (lhs_len <= 0 || rhs_len <= 0) {
		return 0.0;
	}

	int64_t dot_product = dot(p_other);
	// Dot product of 0 means the two vectors are perpendicular
	if (dot_product == 0) {
		return Math::PI / 2.0;
	}
	return Math::acos((double)dot_product / (lhs_len * rhs_len));
}

/**
 * @brief Finds the dot product of two vectors. This allows us to compare how similar one vector's direction is to
 * another - a value equal to the product of their lengths suggests an angle of 0, a value of zero suggest an angle of
 * PI / 2, and a value equal to the negative product of their lengths suggests an angle of PI, and so on. Angles are
 * clamped between -PI and PI, as the method looks for the smallest possible angle between the two.
 * @param p_other The other vector to compare against
 * @returns The dot product of the two vectors
 */
int64_t Vector3i::dot(const Vector3i &p_other) const {
	int64_t ret = 0.0;
	ret += x * p_other.x;
	ret += y * p_other.y;
	ret += z * p_other.z;
	return ret;
}

/**
 * @brief Returns a vector that is orthogonal to the two given vectors. The resulting vector in our case will follow
 * the right-hand rule.
 * @param p_other The other vector to multiply against
 * @returns The cross product of the two vectors
 */
Vector3i Vector3i::cross(const Vector3i &p_other) const {
	Vector3i ret;
	ret.x = (y * p_other.z) - (z * p_other.y);
	ret.y = (z * p_other.x) - (x * p_other.z);
	ret.z = (x * p_other.y) - (y * p_other.x);
	return ret;
}

/**
 * @brief Gets the inverse of a vector
 */
Vector3i Vector3i::inverse() const {
	return Vector3i(-x, -y, -z);
}

/**
 * @brief Normalizes the current vector, which is where one divides each component by the magnitude of the vector.
 */
void Vector3i::normalize() {
	int64_t len = length();

	if (len == 0) {
		return;
	}

	*this /= len;
}

/**
 * @brief Creates a vector that is a normalized version of the current vector.
 * @returns A normalized version of the given vector
 */
Vector3i Vector3i::normalized() const {
	Vector3i ret = *this;
	ret.normalize();
	return ret;
}

/**
 * @brief Obtains a `Vector3i` that is linearly interpolated between two points defined by the current `Vector3i` and
 * the one passed in. For further information on linear interpolation, see `Math::lerp()`.
 * @param p_to The other `Vector3i` to interpolate towards.
 * @param p_factor The factor by which the result is interpolated. 0 is the current `Vector3i` and 1 is the desination.
 * @return A `Vector3i` linearly interpolated by the given factor.
 */
Vector3i Vector3i::lerp(const Vector3i &p_to, double p_factor) const {
	Vector3i ret;
	ret.x = (int)Math::lerp((double)x, (double)p_to.x, p_factor);
	ret.y = (int)Math::lerp((double)y, (double)p_to.y, p_factor);
	ret.z = (int)Math::lerp((double)z, (double)p_to.z, p_factor);
	return ret;
}

/**
 * @brief Checks if the current vector's magnitude is equal to 1.
 */
bool Vector3i::is_normalized() const {
	return Math::is_equal(length(), 1);
}

/**
 * @brief Compares the current vector with another vector to see if they are approximately the same.
 */
bool Vector3i::is_equal(const Vector3i &p_other) const {
	return Math::is_equal(x, p_other.x) && Math::is_equal(y, p_other.y) && Math::is_equal(z, p_other.z);
}
