#include "core/math/vector2i.h"
#include "core/math/vector2.h"
#include "core/string/vstring.h"

String Vector2i::stringify() const {
    String ret;
    ret += "(" + itos(x) + ", " + itos(y) + ")";
    return ret;
}

Vector2i::operator String() const {
    return stringify();
}

Vector2i::operator Vector2() const {
    return Vector2(x, y);
}
