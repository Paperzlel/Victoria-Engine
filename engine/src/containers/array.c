#include "array.h"

#include "core/vmemory.h"
#include "core/logger.h"

#include "maths/maths-types.h"

Array CreateArray(u64 elementSize, u64 elements, ArrayTypes type) {
    Array outArr;
    switch (type) {
        case ARRAY:
            outArr.contents = VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
            break;
        case STRING_ARRAY:
            outArr.contents = (char**)VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
            break;
        case BYTE_ARRAY:
            outArr.contents = VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
            break;
        case I32_ARRAY:
            outArr.contents = (i32*)VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
            break;
        case F32_ARRAY:
            outArr.contents = (f32*)VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
            break;
        case VEC2_ARRAY:
            outArr.contents = (vec2*)VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
            break;
        case VEC3_ARRAY:
            outArr.contents = (vec3*)VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
            break;
        case VEC4_ARRAY:
            outArr.contents = (vec4*)VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
            break;
        case MAT4_ARRAY:
            outArr.contents = (mat4*)VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
            break;
        default:
            outArr.contents = VAllocate(elementSize * elements, MEMORY_TAG_ARRAY);
    }
    outArr.size = elements * elementSize;
    outArr.stride = elementSize;
    return outArr;
}

void DestroyArray(Array* a) {
    VFree(a->contents, a->size, MEMORY_TAG_ARRAY);
    a->elementCount = 0;
    a->size = 0;
    a->stride = 0;
}

StringArray CreateStringArray(u64 elements) {
    StringArray outArr;
    outArr.contents = (char**)VAllocate(sizeof(char*) * elements, MEMORY_TAG_ARRAY);
    for (i32 i = 0; i < elements; i++) {
        outArr.contents[i] = "";
    }
    outArr.size = sizeof(char*) * elements;
    outArr.stride = sizeof(char*);
    outArr.elementCount = elements;
    return outArr;
}

void DestroyStringArray(StringArray* a) {
    VFree(a->contents, a->size, MEMORY_TAG_ARRAY);
    a->elementCount = 0;
    a->size = 0;
    a->stride = 0;
}

b8 ResizeStringArray(StringArray* a, u64 newSize) {
    a->contents = (char**)VReallocate(a->contents, a->size, newSize, MEMORY_TAG_ARRAY);
    a->size = newSize;
    a->elementCount += (newSize / sizeof(char*));
    return TRUE;
}