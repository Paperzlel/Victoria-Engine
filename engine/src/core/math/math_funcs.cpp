#include <core/math/math_funcs.h>

#include <math.h>

/**
 * @brief Returns a value to a given power.
 */
double Math::pow(double p_val, double pow) {
    return ::pow(p_val, pow);
}

/**
 * @brief Returns the square root of the value given.
 */
double Math::sqrt(double p_val) {
    return ::sqrt(p_val);
}

/**
 * @brief Returns the absolute value of the given value.
 */
double Math::abs(double p_val) {
    return fabs(p_val);
}

/**
 * @brief Returns the sin value for the given angle.
 */
double Math::sin(double p_theta) {
    return ::sin(p_theta);
}

/**
 * @brief Returns the cosine value of the given angle.
 */
double Math::cos(double p_theta) {
    return ::cos(p_theta);
}

/**
 * @brief Returns the tan value of the given angle.
 */
double Math::tan(double p_theta) {
    return ::tan(p_theta);
}

/**
 * @brief Returns the angle for the given inverse sin value.
 */
double Math::asin(double p_val) {
    return p_val < -1 ? -PI / 2 : (p_val > 1 ? PI / 2 : ::asin(p_val));
}

/**
 * @brief Returns the angle for the given inverse cosine value.
 */
double Math::acos(double p_val) {
    return p_val < -1 ? PI : (p_val > 1 ? 0 : ::acos(p_val));
}

/**
 * @brief Returns the angle for the given inverse tan value.
 */
double Math::atan(double p_val) {
    return ::atan(p_val);
}

/**
 * @brief Returns the angle of a point `(x, y)` from the origin between -PI and PI. 
 */
double Math::atan2(double p_x, double p_y) {
    return ::atan2(p_x, p_y);
}