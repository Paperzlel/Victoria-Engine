#include "core/math/math_funcs.h"

#include <math.h>

double Math::pow(double p_val, double pow) {
	return ::pow(p_val, pow);
}

double Math::sqrt(double p_val) {
	return ::sqrt(p_val);
}

double Math::abs(double p_val) {
	return fabs(p_val);
}

double Math::sin(double p_theta) {
	return ::sin(p_theta);
}

double Math::cos(double p_theta) {
	return ::cos(p_theta);
}

double Math::tan(double p_theta) {
	return ::tan(p_theta);
}

double Math::asin(double p_val) {
	return p_val < -1 ? -PI / 2 : (p_val > 1 ? PI / 2 : ::asin(p_val));
}

double Math::acos(double p_val) {
	return p_val < -1 ? PI : (p_val > 1 ? 0 : ::acos(p_val));
}

double Math::atan(double p_val) {
	return ::atan(p_val);
}

double Math::atan2(double p_x, double p_y) {
	return ::atan2(p_x, p_y);
}

double Math::floor(double p_x) {
	return ::floor(p_x);
}

double Math::ceil(double p_x) {
	return ::ceil(p_x);
}

double Math::log10(double p_x) {
	return ::log10(p_x);
}

double Math::log2(double p_x) {
	return ::log2(p_x);
}

double Math::ln(double p_x) {
	return ::log(p_x);
}
