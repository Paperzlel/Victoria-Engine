#pragma once

#include "math_funcs.h"

#include "core/typedefs.h"

class String;
struct Vector2i;

/**
 * @brief Structure that represents a 2D vector, which is often used to describe motion with regards to the mouse or
 * items within a 2D canvas.
 */
struct VAPI Vector2 {
public:
	union {
		double elements[2] = {0};
		struct {
			// The x component of the vector
			double x;
			// The y component of the vector
			double y;
		};
	};

	FORCE_INLINE double &operator[](int p_index) {
		return elements[CLAMP(p_index, 0, 1)];
	}

	FORCE_INLINE const double &operator[](int p_index) const {
		return elements[CLAMP(p_index, 0, 1)];
	}

	/**
	 * @brief Assignment operator for a `Vector2`. Copies the x and y components from one into the other.
	 */
	FORCE_INLINE void operator=(const Vector2 &p_other) {
		x = p_other.x;
		y = p_other.y;
	}

	FORCE_INLINE bool operator==(const Vector2 &p_other) const;
	FORCE_INLINE bool operator!=(const Vector2 &p_other) const;

	FORCE_INLINE Vector2 operator+(const Vector2 &p_other);
	FORCE_INLINE Vector2 &operator+=(const Vector2 &p_other);
	FORCE_INLINE Vector2 operator-(const Vector2 &p_other);
	FORCE_INLINE Vector2 &operator-=(const Vector2 &p_other);
	FORCE_INLINE Vector2 operator*(const Vector2 &p_other);
	FORCE_INLINE Vector2 operator*(const double p_scalar);
	FORCE_INLINE Vector2 &operator*=(const Vector2 &p_other);
	FORCE_INLINE Vector2 &operator*=(const double p_scalar);
	FORCE_INLINE Vector2 operator/(const Vector2 &p_other);
	FORCE_INLINE Vector2 operator/(const double p_scalar);
	FORCE_INLINE Vector2 &operator/=(const Vector2 &p_other);
	FORCE_INLINE Vector2 &operator/=(const double p_scalar);

	/**
	 * @brief Represents a vector with a magnitude of 0.
	 */
	FORCE_INLINE static Vector2 zero() {
		return Vector2(0, 0);
	}

	/**
	 * @brief Represents a vector with each component set to 1.
	 */
	FORCE_INLINE static Vector2 one() {
		return Vector2(1, 1);
	}

	/**
	 * @brief Represents a unit vector which points along the positive y axis.
	 */
	FORCE_INLINE static Vector2 up() {
		return Vector2(0, 1);
	}

	/**
	 * @brief Represents a unit vector which points along the negative y axis.
	 */
	FORCE_INLINE static Vector2 down() {
		return Vector2(0, -1);
	}

	/**
	 * @brief Represents a unit vector which points along the negative x axis.
	 */
	FORCE_INLINE static Vector2 left() {
		return Vector2(-1, 0);
	}

	/**
	 * @brief Represents a unit vector which points along the positive x axis.
	 */
	FORCE_INLINE static Vector2 right() {
		return Vector2(1, 0);
	}

	FORCE_INLINE double length() const;
	FORCE_INLINE double length_squared() const;
	FORCE_INLINE double angle(const Vector2 &p_other) const;
	FORCE_INLINE double dot(const Vector2 &p_other) const;
	FORCE_INLINE Vector2 inverse() const;
	FORCE_INLINE Vector2 lerp(const Vector2 &p_to, double p_factor) const;

	FORCE_INLINE void normalize();
	FORCE_INLINE Vector2 normalized();

	String stringify() const;

	operator String() const;
	operator Vector2i() const;

	/**
	 * @brief `Vector2` class constructor.
	 */
	Vector2() {}

	/**
	 * @brief `Vector2` class constructor. Assigns data from another vector to itself.
	 * @param p_other The other vector to copy data from
	 */
	FORCE_INLINE Vector2(const Vector2 &p_other) {
		*this = p_other;
	}

	/**
	 * @brief `Vector2` class constructor. Creates a vector from two specified x and y components.
	 */
	FORCE_INLINE Vector2(double p_x, double p_y) {
		x = p_x;
		y = p_y;
	}
};

/**
 * @brief Equivalence operator for a `Vector2`.
 */
bool Vector2::operator==(const Vector2 &p_other) const {
	return x == p_other.x && y == p_other.y;
}

/**
 * @brief Equivalence operator for a `Vector2`.
 */
bool Vector2::operator!=(const Vector2 &p_other) const {
	return x != p_other.x || y != p_other.y;
}

/**
 * @brief Addition operator for a `Vector2`.
 */
Vector2 Vector2::operator+(const Vector2 &p_other) {
	Vector2 ret = *this;
	ret += p_other;
	return ret;
}

/**
 * @brief Addition operator for a `Vector2`.
 */
Vector2 &Vector2::operator+=(const Vector2 &p_other) {
	x += p_other.x;
	y += p_other.y;
	return *this;
}

/**
 * @brief Subtraction operator for a `Vector2`.
 */
Vector2 Vector2::operator-(const Vector2 &p_other) {
	Vector2 ret = *this;
	ret -= p_other;
	return ret;
}

/**
 * @brief Subtraction operator for a `Vector2`.
 */
Vector2 &Vector2::operator-=(const Vector2 &p_other) {
	x -= p_other.x;
	y -= p_other.y;
	return *this;
}

/**
 * @brief Multiplication operator for a `Vector2`.
 */
Vector2 Vector2::operator*(const Vector2 &p_other) {
	Vector2 ret = *this;
	ret *= p_other;
	return ret;
}

/**
 * @brief Multiplication operator for a `Vector2`, using a scalar.
 */
Vector2 Vector2::operator*(const double p_scalar) {
	Vector2 ret = *this;
	ret *= p_scalar;
	return ret;
}

/**
 * @brief Multiplication operator for a `Vector2`.
 */
Vector2 &Vector2::operator*=(const Vector2 &p_other) {
	x *= p_other.x;
	y *= p_other.y;
	return *this;
}

/**
 * @brief Multiplication operator for a `Vector2`, using a scalar.
 */
Vector2 &Vector2::operator*=(const double p_scalar) {
	x *= p_scalar;
	y *= p_scalar;
	return *this;
}

/**
 * @brief Division operator for a `Vector2`.
 */
Vector2 Vector2::operator/(const Vector2 &p_other) {
	Vector2 ret = *this;
	ret /= p_other;
	return ret;
}

/**
 * @brief Division operator for a `Vector2`, using a scalar.
 */
Vector2 Vector2::operator/(const double p_scalar) {
	Vector2 ret = *this;
	ret /= p_scalar;
	return ret;
}

/**
 * @brief Division operator for a `Vector2`.
 */
Vector2 &Vector2::operator/=(const Vector2 &p_other) {
	x /= p_other.x;
	y /= p_other.y;
	return *this;
}

/**
 * @brief Division operator for a `Vector2`, using a scalar
 */
Vector2 &Vector2::operator/=(const double p_scalar) {
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
double Vector2::length() const {
	return Math::sqrt(length_squared());
}

/**
 * @brief Obtains the squared length of the current vector, which is equivalent to the square of its x component added
 * to the sqaure of its y component.
 */
double Vector2::length_squared() const {
	return x * x + y * y;
}

/**
 * @brief Finds the angle between two vectors, in radians.
 * @param p_other The other vector to find the angle between
 */
double Vector2::angle(const Vector2 &p_other) const {
	double lhs_len = length();
	double rhs_len = p_other.length();

	// <= is not needed because imaginary numbers are not included in this application but god knows what the C++
	// compiler is capable of
	if (lhs_len <= 0 || rhs_len <= 0) {
		return 0.0;
	}

	double dot_product = dot(p_other);
	// Zero messes up the equations for some reason so return PI/2 here
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
double Vector2::dot(const Vector2 &p_other) const {
	double ret = 0.0;
	ret += x * p_other.x;
	ret += y * p_other.y;
	return ret;
}

/**
 * @brief Returns the inverse of the current vector.
 */
Vector2 Vector2::inverse() const {
	return Vector2(-x, -y);
}

/**
 * @brief Obtains a `Vector2` that is linearly interpolated between two points defined by the current `Vector2` and the
 * one passed in. For further information on linear interpolation, see `Math::lerp()`.
 * @param p_to The other `Vector2` to interpolate towards.
 * @param p_factor The factor by which the result is interpolated. 0 is the current `Vector2` and 1 is the desination.
 * @return A `Vector2` linearly interpolated by the given factor.
 */
Vector2 Vector2::lerp(const Vector2 &p_to, double p_factor) const {
	Vector2 ret;
	ret.x = Math::lerp(x, p_to.x, p_factor);
	ret.y = Math::lerp(y, p_to.y, p_factor);
	return ret;
}

/**
 * @brief Normalizes a vector, which is to divide each component by its length to make its magnitude equal to 1.
 */
void Vector2::normalize() {
	double len = length();

	if (len == 0) {
		return;
	}

	x /= len;
	y /= len;
}

/**
 * @brief Obtains a normalized version of the given vector.
 */
Vector2 Vector2::normalized() {
	Vector2 ret = *this;
	ret.normalize();
	return ret;
}
