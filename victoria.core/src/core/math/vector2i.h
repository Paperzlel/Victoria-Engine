#pragma once

#include "math_funcs.h"

#include "core/typedefs.h"

class String;
struct Vector2;

/**
 * @brief Structure that represents a 2D vector, which is often used to describe motion with regards to the mouse or
 * items within a 2D canvas.
 */
struct VAPI Vector2i {
public:
	union {
		int64_t elements[2] = {0};
		struct {
			// The x component of the vector
			int64_t x;
			// The y component of the vector
			int64_t y;
		};
	};

	FORCE_INLINE int64_t &operator[](int p_index) {
		return elements[CLAMP(p_index, 0, 1)];
	}

	FORCE_INLINE const int64_t &operator[](int p_index) const {
		return elements[CLAMP(p_index, 0, 1)];
	}

	/**
	 * @brief Assignment operator for a `Vector2i`. Copies the x and y components from one into the other.
	 */
	FORCE_INLINE void operator=(const Vector2i &p_other) {
		x = p_other.x;
		y = p_other.y;
	}

	FORCE_INLINE bool operator==(const Vector2i &p_other) const;
	FORCE_INLINE bool operator!=(const Vector2i &p_other) const;

	FORCE_INLINE Vector2i operator+(const Vector2i &p_other);
	FORCE_INLINE Vector2i &operator+=(const Vector2i &p_other);
	FORCE_INLINE Vector2i operator-(const Vector2i &p_other);
	FORCE_INLINE Vector2i &operator-=(const Vector2i &p_other);
	FORCE_INLINE Vector2i operator*(const Vector2i &p_other);
	FORCE_INLINE Vector2i operator*(const int64_t p_scalar);
	FORCE_INLINE Vector2i &operator*=(const Vector2i &p_other);
	FORCE_INLINE Vector2i &operator*=(const int64_t p_scalar);
	FORCE_INLINE Vector2i operator/(const Vector2i &p_other);
	FORCE_INLINE Vector2i operator/(const int64_t p_scalar);
	FORCE_INLINE Vector2i &operator/=(const Vector2i &p_other);
	FORCE_INLINE Vector2i &operator/=(const int64_t p_scalar);

	/**
	 * @brief Represents a vector with a magnitude of 0.
	 */
	FORCE_INLINE static Vector2i zero() {
		return Vector2i(0, 0);
	}

	/**
	 * @brief Represents a vector with each component set to 1.
	 */
	FORCE_INLINE static Vector2i one() {
		return Vector2i(1, 1);
	}

	/**
	 * @brief Represents a unit vector which points along the positive y axis.
	 */
	FORCE_INLINE static Vector2i up() {
		return Vector2i(0, 1);
	}

	/**
	 * @brief Represents a unit vector which points along the negative y axis.
	 */
	FORCE_INLINE static Vector2i down() {
		return Vector2i(0, -1);
	}

	/**
	 * @brief Represents a unit vector which points along the negative x axis.
	 */
	FORCE_INLINE static Vector2i left() {
		return Vector2i(-1, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the positive x axis.
	 */
	FORCE_INLINE static Vector2i right() {
		return Vector2i(1, 0);
	}

	FORCE_INLINE double length() const;
	FORCE_INLINE int64_t length_squared() const;
	FORCE_INLINE Vector2i inverse() const;
	FORCE_INLINE Vector2i lerp(const Vector2i &p_to, double p_factor) const;

	FORCE_INLINE void normalize();
	FORCE_INLINE Vector2i normalized();

	String stringify() const;

	operator String() const;
	operator Vector2() const;

	/**
	 * @brief `Vector2i` class constructor.
	 */
	Vector2i() {}

	/**
	 * @brief `Vector2i` class constructor. Assigns data from another vector to itself.
	 * @param p_other The other vector to copy data from
	 */
	FORCE_INLINE Vector2i(const Vector2i &p_other) {
		*this = p_other;
	}

	/**
	 * @brief `Vector2i` class constructor. Creates a vector from two specified x and y components.
	 */
	FORCE_INLINE Vector2i(int64_t p_x, int64_t p_y) {
		x = p_x;
		y = p_y;
	}
};

/**
 * @brief Equivalence operator for a `Vector2i`.
 */
bool Vector2i::operator==(const Vector2i &p_other) const {
	return x == p_other.x && y == p_other.y;
}

/**
 * @brief Equivalence operator for a `Vector2i`.
 */
bool Vector2i::operator!=(const Vector2i &p_other) const {
	return x != p_other.x || y != p_other.y;
}

/**
 * @brief Addition operator for a `Vector2i`.
 */
Vector2i Vector2i::operator+(const Vector2i &p_other) {
	Vector2i ret = *this;
	ret += p_other;
	return ret;
}

/**
 * @brief Addition operator for a `Vector2i`.
 */
Vector2i &Vector2i::operator+=(const Vector2i &p_other) {
	x += p_other.x;
	y += p_other.y;
	return *this;
}

/**
 * @brief Subtraction operator for a `Vector2i`.
 */
Vector2i Vector2i::operator-(const Vector2i &p_other) {
	Vector2i ret = *this;
	ret -= p_other;
	return ret;
}

/**
 * @brief Subtraction operator for a `Vector2i`.
 */
Vector2i &Vector2i::operator-=(const Vector2i &p_other) {
	x -= p_other.x;
	y -= p_other.y;
	return *this;
}

/**
 * @brief Multiplication operator for a `Vector2i`.
 */
Vector2i Vector2i::operator*(const Vector2i &p_other) {
	Vector2i ret = *this;
	ret *= p_other;
	return ret;
}

/**
 * @brief Multiplication operator for a `Vector2i`, using a scalar.
 */
Vector2i Vector2i::operator*(const int64_t p_scalar) {
	Vector2i ret = *this;
	ret *= p_scalar;
	return ret;
}

/**
 * @brief Multiplication operator for a `Vector2i`.
 */
Vector2i &Vector2i::operator*=(const Vector2i &p_other) {
	x *= p_other.x;
	y *= p_other.y;
	return *this;
}

/**
 * @brief Multiplication operator for a `Vector2i`, using a scalar.
 */
Vector2i &Vector2i::operator*=(const int64_t p_scalar) {
	x *= p_scalar;
	y *= p_scalar;
	return *this;
}

/**
 * @brief Division operator for a `Vector2i`.
 */
Vector2i Vector2i::operator/(const Vector2i &p_other) {
	Vector2i ret = *this;
	ret /= p_other;
	return ret;
}

/**
 * @brief Division operator for a `Vector2i`, using a scalar.
 */
Vector2i Vector2i::operator/(const int64_t p_scalar) {
	Vector2i ret = *this;
	ret /= p_scalar;
	return ret;
}

/**
 * @brief Division operator for a `Vector2i`.
 */
Vector2i &Vector2i::operator/=(const Vector2i &p_other) {
	x /= p_other.x;
	y /= p_other.y;
	return *this;
}

/**
 * @brief Division operator for a `Vector2i`, using a scalar
 */
Vector2i &Vector2i::operator/=(const int64_t p_scalar) {
	x /= p_scalar;
	y /= p_scalar;
	return *this;
}

/**
 * @brief Obtains the length of the current vector, which is the shortest possible path between the vector's origin and
 * the coordinate represented by its x and y component. Unlike the `length_squared()` method, this one returns the
 * length in real space.
 * @returns The length of the vector
 */
double Vector2i::length() const {
	return Math::sqrt(length_squared());
}

/**
 * @brief Obtains the squared length of the current vector, which is equivalent to the square of its x component added
 * to the sqaure of its y component.
 */
int64_t Vector2i::length_squared() const {
	return x * x + y * y;
}

/**
 * @brief Returns the inverse of the current vector.
 */
Vector2i Vector2i::inverse() const {
	return Vector2i(-x, -y);
}

/**
 * @brief Obtains a `Vector2i` that is linearly interpolated between two points defined by the current `Vector2i` and
 * the one passed in. For further information on linear interpolation, see `Math::lerp()`.
 * @param p_to The other `Vector2i` to interpolate towards.
 * @param p_factor The factor by which the result is interpolated. 0 is the current `Vector2i` and 1 is the desination.
 * @return A `Vector2i` linearly interpolated by the given factor.
 */
Vector2i Vector2i::lerp(const Vector2i &p_to, double p_factor) const {
	Vector2i ret;
	ret.x = (int64_t)Math::lerp((double)x, (double)p_to.x, p_factor);
	ret.y = (int64_t)Math::lerp((double)y, (double)p_to.y, p_factor);
	return ret;
}

/**
 * @brief Normalizes a vector, which is to divide each component by its length to make its magnitude equal to 1.
 */
void Vector2i::normalize() {
	int64_t len = length();

	if (len == 0) {
		return;
	}

	x /= len;
	y /= len;
}

/**
 * @brief Obtains a normalized version of the given vector.
 */
Vector2i Vector2i::normalized() {
	Vector2i ret = *this;
	ret.normalize();
	return ret;
}
