#pragma once 

#include "defines.h"

/* Vector2 */
typedef union Vector2Union {
    f32 elements[2];
    struct {
        union {
            f32 x;
        };
        union {
            f32 y;
        };
    };
} vec2;

/* Vector3 */
typedef union Vector3Union {
    f32 elements[3];
    struct {
        union {
            f32 x, r, h;
        };
        union {
            f32 y, g, s;
        };
        union {
            f32 z, b, v;
        };
    };
} vec3;

typedef union Vector4Union {
    f32 elements[4];
    struct {
        union {
            f32 x, r;
        };
        union {
            f32 y, g;
        };
        union {
            f32 z, b;
        };
        union {
            f32 w, a;
        };
    };
} vec4;

/* A 4x4 matrix */
typedef union mat4Union {
    f32 data[16];
} mat4;

