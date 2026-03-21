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

	static double pow(double p_val, double pow);
	static double sqrt(double p_val);

	static double abs(double p_val);

	static double sin(double p_theta);
	static double cos(double p_theta);
	static double tan(double p_theta);
	static double asin(double p_val);
	static double acos(double p_val);
	static double atan(double p_val);
	static double atan2(double p_x, double p_y);

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
