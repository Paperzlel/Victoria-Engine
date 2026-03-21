#include "core/math/vector2.h"

#include "core/math/vector2i.h"
#include "core/string/vstring.h"

/**
 * @brief Transfers a vector from itself into a string, which can be used to debug information in a console, or for
 * some other purpose.
 * @returns The current vector represented as a string.
 */
String Vector2::stringify() const {
	String ret;
	ret += "(" + ftos(x) + ", " + ftos(y) + ")";
	return ret;
}

Vector2::operator String() const {
	return stringify();
}

Vector2::operator Vector2i() const {
	return Vector2i((i64)x, (i64)y);
}