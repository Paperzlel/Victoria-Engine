#include "core/math/vector3.h"

#include "core/math/vector3i.h"
#include "core/string/vstring.h"

/**
 * @brief Take the current vector and makes it into a string, which can then be printed to the console if needed.
 * @returns The current vector as a string
 */
String Vector3::stringify() const {
	String ret;
	ret += "(" + ftos(x) + ", " + ftos(y) + ", " + ftos(z) + ")";
	return ret;
}

Vector3::operator String() const {
	return stringify();
}

Vector3::operator Vector3i() const {
	return Vector3i((int64_t)x, (int64_t)y, (int64_t)z);
}