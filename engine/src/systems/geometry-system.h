#pragma once

#include "defines.h"
#include "maths/maths-types.h"

typedef struct Triangle2D {
    u64 id;
    mat2 position;
} TRI_2D;

typedef struct Triangle3D {
    u32 id;
    mat3 position;
} TRI_3D;
