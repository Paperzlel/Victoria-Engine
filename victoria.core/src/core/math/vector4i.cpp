#include "core/math/vector4i.h"

#include "core/math/vector4.h"
#include "core/string/vstring.h"

String Vector4i::stringify() const {
	String ret;
	ret += "(" + itos(x) + ", " + itos(y) + ", " + itos(z) + ", " + itos(w) + ")";
	return ret;
}

Vector4i::operator String() const {
	return stringify();
}

Vector4i::operator Vector4() const {
	return Vector4(x, y, z, w);
}