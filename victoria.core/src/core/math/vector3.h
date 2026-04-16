#pragma once

#include "math_funcs.h"

#include "core/typedefs.h"

class String;
struct Vector3i;

/**
 * @brief Struct that represents a vector in 3D space.
 */
struct VAPI Vector3 {
public:
	union {
		double elements[3] = {0};
		struct {
			double x;
			double y;
			double z;
		};
	};

	FORCE_INLINE double &operator[](int p_index) {
		return elements[CLAMP(p_index, 0, 2)];
	}

	FORCE_INLINE const double &operator[](int p_index) const {
		return elements[CLAMP(p_index, 0, 2)];
	}

	FORCE_INLINE void operator=(const Vector3 &p_other) {
		x = p_other.x;
		y = p_other.y;
		z = p_other.z;
	}

	FORCE_INLINE bool operator==(const Vector3 &p_other) const;
	FORCE_INLINE bool operator!=(const Vector3 &p_other) const;

	FORCE_INLINE Vector3 operator+(const Vector3 &p_other) const;
	FORCE_INLINE Vector3 &operator+=(const Vector3 &p_other);
	FORCE_INLINE Vector3 operator-(const Vector3 &p_other) const;
	FORCE_INLINE Vector3 &operator-=(const Vector3 &p_other);
	FORCE_INLINE Vector3 operator*(const Vector3 &p_other) const;
	FORCE_INLINE Vector3 operator*(double p_scalar) const;
	FORCE_INLINE Vector3 &operator*=(const Vector3 &p_other);
	FORCE_INLINE Vector3 &operator*=(double p_scalar);
	FORCE_INLINE Vector3 operator/(const Vector3 &p_other) const;
	FORCE_INLINE Vector3 operator/(double p_scalar) const;
	FORCE_INLINE Vector3 &operator/=(const Vector3 &p_other);
	FORCE_INLINE Vector3 &operator/=(double p_scalar);

	/**
	 * @brief Represents a vector with each component set to 0.
	 */
	static FORCE_INLINE Vector3 zero() {
		return Vector3(0, 0, 0);
	}

	/**
	 * @brief Represents a vector with each component set to 1.
	 */
	static FORCE_INLINE Vector3 one() {
		return Vector3(1, 1, 1);
	}

	/**
	 * @brief Represents a unit vector which points along the positive y-axis.
	 */
	static FORCE_INLINE Vector3 up() {
		return Vector3(0, 1, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the negative y-axis.
	 */
	static FORCE_INLINE Vector3 down() {
		return Vector3(0, -1, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the positive x-axis.
	 */
	static FORCE_INLINE Vector3 left() {
		return Vector3(-1, 0, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the negative x-axis.
	 */
	static FORCE_INLINE Vector3 right() {
		return Vector3(1, 0, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the negative z-axis (since we use a right-handed coordinate
	 * system, positive Z points into the screen)
	 */
	static FORCE_INLINE Vector3 forward() {
		return Vector3(0, 0, -1);
	}

	/**
	 * @brief Represents a unit vector which points along the positive z-axis (since we use a right-handed coordinate
	 * system, positive Z points into the screen)
	 */
	static FORCE_INLINE Vector3 backward() {
		return Vector3(0, 0, 1);
	}

	FORCE_INLINE double length() const;
	FORCE_INLINE double length_squared() const;
	FORCE_INLINE double angle(const Vector3 &p_other) const;
	FORCE_INLINE double dot(const Vector3 &p_other) const;
	FORCE_INLINE Vector3 cross(const Vector3 &p_other) const;
	FORCE_INLINE Vector3 inverse() const;
	FORCE_INLINE void normalize();
	FORCE_INLINE Vector3 normalized() const;
	FORCE_INLINE Vector3 lerp(const Vector3 &p_to, double p_factor) const;
	String stringify() const;

	FORCE_INLINE bool is_normalized() const;
	FORCE_INLINE bool is_equal(const Vector3 &p_other) const;

	operator String() const;
	operator Vector3i() const;

	Vector3() {}
	FORCE_INLINE Vector3(const Vector3 &p_other) {
		*this = p_other;
	}
	FORCE_INLINE Vector3(double p_x, double p_y, double p_z) {
		x = p_x;
		y = p_y;
		z = p_z;
	}
};

bool Vector3::operator==(const Vector3 &p_other) const {
	return x == p_other.x && y == p_other.y && z == p_other.z;
}

bool Vector3::operator!=(const Vector3 &p_other) const {
	return x != p_other.x || y != p_other.y || z != p_other.z;
}

Vector3 Vector3::operator+(const Vector3 &p_other) const {
	Vector3 ret = *this;
	ret += p_other;
	return ret;
}

Vector3 &Vector3::operator+=(const Vector3 &p_other) {
	x += p_other.x;
	y += p_other.y;
	z += p_other.z;
	return *this;
}

Vector3 Vector3::operator-(const Vector3 &p_other) const {
	Vector3 ret = *this;
	ret -= p_other;
	return ret;
}

Vector3 &Vector3::operator-=(const Vector3 &p_other) {
	x -= p_other.x;
	y -= p_other.y;
	z -= p_other.z;
	return *this;
}

Vector3 Vector3::operator*(const Vector3 &p_other) const {
	Vector3 ret = *this;
	ret *= p_other;
	return ret;
}

Vector3 Vector3::operator*(double p_scalar) const {
	Vector3 ret = *this;
	ret *= p_scalar;
	return ret;
}

Vector3 &Vector3::operator*=(const Vector3 &p_other) {
	x *= p_other.x;
	y *= p_other.y;
	z *= p_other.z;
	return *this;
}

Vector3 &Vector3::operator*=(double p_scalar) {
	x *= p_scalar;
	y *= p_scalar;
	z *= p_scalar;
	return *this;
}

Vector3 Vector3::operator/(const Vector3 &p_other) const {
	Vector3 ret = *this;
	ret /= p_other;
	return ret;
}

Vector3 Vector3::operator/(double p_scalar) const {
	Vector3 ret = *this;
	ret /= p_scalar;
	return ret;
}

Vector3 &Vector3::operator/=(const Vector3 &p_other) {
	x /= p_other.x;
	y /= p_other.y;
	z /= p_other.z;
	return *this;
}

Vector3 &Vector3::operator/=(double p_scalar) {
	x /= p_scalar;
	y /= p_scalar;
	z /= p_scalar;
	return *this;
}

/**
 * @brief Gets the length of a given vector.
 */
double Vector3::length() const {
	return Math::sqrt(length_squared());
}

/**
 * @brief Gets the square of the magnitude of a given vector.
 */
double Vector3::length_squared() const {
	return x * x + y * y + z * z;
}

/**
 * @brief Gets the smallest angle between two given vectors (between `-PI` and `PI`).
 * @param p_other The other vector to find the angle between
 */
double Vector3::angle(const Vector3 &p_other) const {
	double lhs_len = length();
	double rhs_len = p_other.length();

	// <= is not needed because imaginary numbers are not included in this application but god knows what the C++
	// compiler is capable of
	if (lhs_len <= 0 || rhs_len <= 0) {
		return 0.0;
	}

	double dot_product = dot(p_other);
	// Dot product of 0 means the two vectors are perpendicular
	if (dot_product == 0) {
		return Math::PI / 2.0;
	}
	return Math::acos(dot_product / (lhs_len * rhs_len));
}

/**
 * @brief Finds the dot product of two vectors. This allows us to compare how similar one vector's direction is to
 * another - a value equal to the product of their lengths suggests an angle of 0, a value of zero suggest an angle of
 * PI / 2, and a value equal to the negative product of their lengths suggests an angle of PI, and so on. Angles are
 * clamped between -PI and PI, as the method looks for the smallest possible angle between the two.
 * @param p_other The other vector to compare against
 * @returns The dot product of the two vectors
 */
double Vector3::dot(const Vector3 &p_other) const {
	double ret = 0.0;
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
Vector3 Vector3::cross(const Vector3 &p_other) const {
	Vector3 ret;
	ret.x = (y * p_other.z) - (z * p_other.y);
	ret.y = (z * p_other.x) - (x * p_other.z);
	ret.z = (x * p_other.y) - (y * p_other.x);
	return ret;
}

/**
 * @brief Gets the inverse of a vector
 */
Vector3 Vector3::inverse() const {
	return Vector3(-x, -y, -z);
}

/**
 * @brief Normalizes the current vector, which is where one divides each component by the magnitude of the vector.
 */
void Vector3::normalize() {
	double len = length();

	if (len == 0) {
		return;
	}

	*this /= len;
}

/**
 * @brief Creates a vector that is a normalized version of the current vector.
 * @returns A normalized version of the given vector
 */
Vector3 Vector3::normalized() const {
	Vector3 ret = *this;
	ret.normalize();
	return ret;
}

/**
 * @brief Obtains a `Vector3` that is linearly interpolated between two points defined by the current `Vector3` and
 * the one passed in. For further information on linear interpolation, see `Math::lerp()`.
 * @param p_to The other `Vector3` to interpolate towards.
 * @param p_factor The factor by which the result is interpolated. 0 is the current `Vector3` and 1 is the desination.
 * @return A `Vector3` linearly interpolated by the given factor.
 */
Vector3 Vector3::lerp(const Vector3 &p_to, double p_factor) const {
	Vector3 ret;
	ret.x = Math::lerp(x, p_to.x, p_factor);
	ret.y = Math::lerp(y, p_to.y, p_factor);
	ret.z = Math::lerp(z, p_to.z, p_factor);
	return ret;
}

/**
 * @brief Checks if the current vector's magnitude is equal to 1.
 */
bool Vector3::is_normalized() const {
	return Math::is_equal(length(), 1);
}

/**
 * @brief Compares the current vector with another vector to see if they are approximately the same.
 */
bool Vector3::is_equal(const Vector3 &p_other) const {
	return Math::is_equal(x, p_other.x) && Math::is_equal(y, p_other.y) && Math::is_equal(z, p_other.z);
}

FORCE_INLINE Vector3 operator*(double p_left, const Vector3 &p_right) {
	return p_right * p_left;
}