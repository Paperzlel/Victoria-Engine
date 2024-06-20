#include "vmaths.h"

#include <math.h>

vec2 Vector2Zero() {
    return (vec2){{0.0f, 0.0f}};
}

vec2 Vector2(f32 x, f32 y) {
    return (vec2){{x, y}};
}

f64 vsin(f64 value) {
    return sin(value);
}

f64 vcos(f64 value) {
    return cos(value);
}

f64 vtan(f64 value) {
    return tan(value);
}