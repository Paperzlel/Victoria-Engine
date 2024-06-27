#include "vmaths.h"

#include <math.h>
#include <stdlib.h>

f64 vsin(f64 value) {
    return sin(value);
}

f64 vcos(f64 value) {
    return cos(value);
}

f64 vtan(f64 value) {
    return tan(value);
}

f64 vasin(f64 value) {
    return asin(value);
}

f64 vacos(f64 value) {
    return acos(value);
}

f64 vatan(f64 value) {
    return atan(value);
}

f32 vabs(f32 value) {
    return fabsf(value);
}

f32 vsqrt(f32 value) {
    return sqrt(value);
}