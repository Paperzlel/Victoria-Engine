#pragma once

#include "core/typedefs.h"

// Constant to represent a very small number to compare floating-point values against. The phrase "approximately the
// same" in this documentation refers to if the absolute difference between two values is less than this value.
#define V_EPSILON 0.000001

/**
 * @brief Core math class. Contains all the usual mathematical functions needed across the application.
 */
class VAPI Math {
public:
	// Constant for the value of pi to 15 d.p.
	constexpr static const double PI = 3.141592653589793;
	// Constant for a value to convert degrees into radians
	constexpr static const double DEG_TO_RAD_MULTIPLIER = PI / 180;
	// Constant for a value to convert radians into degrees
	constexpr static const double RAD_TO_DEG_MULTIPLIER = 180 / PI;

	/**
	 * @brief Returns a value to a given power.
	 */
	static double pow(double p_val, double pow);

	/**
	 * @brief Returns the square root of the value given.
	 */
	static double sqrt(double p_val);

	/**
	 * @brief Returns the absolute value of the given value.
	 */
	static double abs(double p_val);

	/**
	 * @brief Returns the sin value for the given angle.
	 */
	static double sin(double p_theta);

	/**
	 * @brief Returns the cosine value of the given angle.
	 */
	static double cos(double p_theta);

	/**
	 * @brief Returns the tan value of the given angle.
	 */
	static double tan(double p_theta);

	/**
	 * @brief Returns the angle for the given inverse sin value.
	 */
	static double asin(double p_val);

	/**
	 * @brief Returns the angle for the given inverse cosine value.
	 */
	static double acos(double p_val);

	/**
	 * @brief Returns the angle for the given inverse tan value.
	 */
	static double atan(double p_val);

	/**
	 * @brief Returns the angle of a point `(x, y)` from the origin between -PI and PI.
	 */
	static double atan2(double p_x, double p_y);

	/**
	 * @brief Rounds the given value down to the nearest integer.
	 * @param p_x The value to round.
	 * @return The value rounded down to the nearest integer.
	 */
	static double floor(double p_x);

	/**
	 * @brief Rounds the given value up to the nearest integer.
	 * @param p_x The value to round.
	 * @return The value rounded up to the nearest integer.
	 */
	static double ceil(double p_x);

	/**
	 * @brief Obtains the base-10 logarithmic value for the given value.
	 * @param p_x The value to find the logarithm for.
	 * @return The base-10 logarithm of the given value.
	 */
	static double log10(double p_x);

	/**
	 * @brief Obtains the base-2 logarithmic value for the given value.
	 * @param p_x The value to find the logarithm of.
	 * @return The base-2 logarithm of the given value.
	 */
	static double log2(double p_x);

	/**
	 * @brief Obtains the natural logarithm for the given value.
	 * @param p_x The value to find the natural logarithm of.
	 * @return The natural logarithm of the given value.
	 */
	static double ln(double p_x);

	/**
	 * @brief Linearly interpolates a value between a start and end position.
	 * @param p_from The starting value
	 * @param p_to The ending value
	 * @param p_delta The amount of the total distance to travel. Between 0 and 1.
	 * @return A position between `p_from` and `p_to`.
	 */
	FORCE_INLINE static double lerp(double p_from, double p_to, double p_delta) {
		return p_from + (p_to - p_from) * p_delta;
	}

	/**
	 * @brief Compares if two values are close enough to one another to be considered equal.
	 * @param p_x The first value one wants to compare
	 * @param p_y The second value one wants to compare
	 */
	static bool is_equal(double p_x, double p_y) {
		if (p_x == p_y) {
			return true;
		}

		return abs(p_x - p_y) < V_EPSILON;
	}

	/**
	 * @brief Compares if a given value is approximately equal to zero.
	 * @param p_x The first value one wants to compare
	 */
	static bool is_zero(double p_x) {
		if (p_x == 0) {
			return true;
		}

		return abs(p_x) < V_EPSILON;
	}
};
