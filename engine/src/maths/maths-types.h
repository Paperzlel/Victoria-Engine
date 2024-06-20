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

/* A 2x2 matrix */
typedef union mat2Union {
    f32 data[4];
} mat2;

/* A 3x3 matrix*/
typedef union mat3Union {
    f32 data[9];
} mat3;

/* A 4x4 matrix */
typedef union mat4Union {
    f32 data[16];
} mat4;

