#include "core/math/vector3i.h"

#include "core/math/vector3.h"
#include "core/string/vstring.h"

String Vector3i::stringify() const {
	String ret;
	ret += "(" + itos(x) + ", " + itos(y) + ", " + itos(z) + ")";
	return ret;
}

Vector3i::operator String() const {
	return stringify();
}

Vector3i::operator Vector3() const {
	return Vector3(x, y, z);
}