#include "core/math/vector4.h"

#include "core/string/vstring.h"
#include "core/math/vector4i.h"

/**
 * @brief Take the current vector and makes it into a string, which can then be printed to the console if needed. 
 * @returns The current vector as a string
 */
String Vector4::stringify() const {
    String ret;
    ret += "(" + ftos(x) + ", " + ftos(y) + ", " + ftos(z) + ", " + ftos(w) + ")";
    return ret;
}

Vector4::operator String() const {
    return stringify();
}

Vector4::operator Vector4i() const {
    return Vector4i((i64)x, (i64)y, (i64)z, (i64)w);
}