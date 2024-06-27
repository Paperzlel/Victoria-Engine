#pragma once

#include "defines.h"

typedef enum ArrayTypes {
    ARRAY,
    STRING_ARRAY,
    BYTE_ARRAY,
    U8_ARRAY,
    U16_ARRAY,
    U32_ARRAY,
    U64_ARRAY,
    I8_ARRAY,
    I16_ARRAY,
    I32_ARRAY,
    I64_ARRAY,
    F32_ARRAY,
    F64_ARRAY,
    VEC2_ARRAY,
    VEC3_ARRAY,
    VEC4_ARRAY,
    MAT4_ARRAY,
    NAN_ARRAY
} ArrayTypes;

/* Non-specific array type */
typedef struct ArrayGeneral {
    void* contents;
    u64 size;
    u64 elementCount;
    u64 stride;
} Array;

/* Array of strings */
typedef struct StringArray {
    char** contents;
    u64 size;
    u64 elementCount;
    u64 stride;
} StringArray;

/*
 * @brief Method to create and return a new array of a given size and type of content
 * @param elementSize The size of the type of data to input in bytes
 * @param elements The number of elements to initially have in the array
 * @return An empty array of the given size and count
 */
Array CreateArray(u64 elementSize, u64 elements, ArrayTypes type);

/*
 * @brief Method to unitialize all of the contents inside the array
 * @param *a Pointer to the array to destroy
 */
void DestroyArray(Array* a);

/*
 * @brief Method to create a StringArray with a given set of elements
 * @param elements The number of elements to enter into the array
 * @return A `StringArray` of the given size
 */
StringArray CreateStringArray(u64 elements);

/* 
 * @brief Method to uninitialize a given `StringArray` 
 * @param *a Pointer to the `StringArray` to unitialize
 */
void DestroyStringArray(StringArray* a);

/* 
 * @brief Method to resize an array to a new size
 * @param *a Pointer to the `StringArray` to resize
 * @param newSize The new size of the `StringArray` in bytes
 * @return `TRUE` if the `StringArray` was resized successfully, `FALSE` if it failed somehow
 */
b8 ResizeStringArray(StringArray* a, u64 newSize);

//TODO: The rest of these functions

b8 ArrayResize(Array* a, i32 newSize);

void* ArraySearch(Array* a, void* item);

Array ArrayFindList(Array* a, void* item);
