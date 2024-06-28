#pragma once

#include "defines.h"

#include "maths-types.h"

#include "core/vmemory.h"

// IMPORTANT NOTE:
// All angles given in these method will be in radians. Later on you will be able to convert between the two, but for now you must use radians

#define PI 3.1415926535897932384f
#define DEG_TO_RAD_MULTIPLIER PI / 180
#define RAD_TO_DEG_MULTIPLIER 180 / PI

/*
 * ===================
 * GENERAL METHODS
 * ===================
 */

/* @brief Method for a sine wave.
 * @param value The x position along the sine wave
 * @return A value between -1 and 1 following a sine wave
 */
VAPI f64 vsin(f64 value);

/* 
 * @brief Method for a cosine wave.
 * @param value The x position along the cosine wave
 * @return A value between -1 and 1 following a cosine wave
 */
VAPI f64 vcos(f64 value);

/* 
 * @brief Method for a tan wave.
 * @param value The x position along the tan wave
 * @return A value between -INF and INF following a tan wave
 */
VAPI f64 vtan(f64 value);

/*
 * @brief Method for getting the angle from a sine wave
 * @param value An x value between -1 and 1
 * @return An angle between 0 and 360 degrees
 */
VAPI f64 vasin(f64 value);

/* 
 * @brief Method for getting the angle from a cosine wave
 * @param value A value between -1 and 1
 * @return An angle between 0 and 360 degrees
 */
VAPI f64 vacos(f64 value);

/*
 * @brief Method for getting the angle from a tan wave
 * @param value A value btween -INF and INF
 * @return An angle between 0 and 360 degrees, excluding 90 and 270
 */
VAPI f64 vatan(f64 value);

/* 
 * @brief Method to give the absolute value of a number
 * @param value The number to get the absolute value of
 * @return An absolute value (number > 0)
 */
VAPI f32 vabs(f32 value);

/*
 * @brief Method to give the square root of a number
 * @param value The value to square root
 * @return The square root of `value`
 */
VAPI f32 vsqrt(f32 value);


/* 
 * ===================
 * VECTOR2 METHODS
 * ===================
 */

/* @brief Method to return a new vec2 with predetermined values */
VINLINE vec2 Vector2(f32 x, f32 y) {
    vec2 outVec;
    outVec.x = x;
    outVec.y = y;
    return outVec;
}

/* @brief Method to return a new Vector2 with no values set */
VINLINE vec2 Vector2Zero() {
    return (vec2){0.0f, 0.0f};
}

/* @brief Method to return a vec2 with all values set to one*/
VINLINE vec2 Vector2One() {
    return (vec2){1.0f, 1.0f};
}

/* @brief Method to return a vec2 pointing upwards */
VINLINE vec2 Vector2Up() {
    return (vec2){0.0f, 1.0f};
}

/* @brief Method to return a vec2 pointing downwards */
VINLINE vec2 Vector2Down() {
    return (vec2){0.0f, -1.0f};
}

/* @brief Method to return a vec2 pointing to the left */
VINLINE vec2 Vector2Left() {
    return (vec2){-1.0f, 0.0f};
}

/* @brief Method to return a vec2 pointing to the right */
VINLINE vec2 Vector2Right() {
    return (vec2){1.0f, 0.0f};
}

/*
 * @brief Method to return the sum of two vec2s
 * @param vecA The first vec2
 * @param vecB The second vec2
 * @return The sum of both vec2s
 */
VINLINE vec2 Vector2Add(vec2 vecA, vec2 vecB) {
    return (vec2)
    {vecA.x + vecA.x,
    vecA.y + vecB.y};
}

/*
 * @brief Method to subtract two vec2s from one another
 * @param vecA The first vec2
 * @param vecB The second vec2
 * @return The difference between the two vec2s
 */
VINLINE vec2 Vector2Sub(vec2 vecA, vec2 vecB) {
    return (vec2)
    {vecA.x - vecB.x,
    vecA.y - vecB.y};
}

/* 
 * @brief Method to multiply two vec2 against one another
 * @param vecA The first vec2
 * @param vecB The second vec2
 * @return The product of both vectors
 */
VINLINE vec2 Vector2Mul(vec2 vecA, vec2 vecB) {
    return (vec2)
    {vecA.x * vecB.x,
    vecA.y * vecB.y};
}

/* 
 * @brief Method to multiply a vec2 by a given scalar
 * @param vector The vec2 to multiply
 * @param scalar The scalar to multiply against
 * @return The product of the vec2 and the scalar
 */
VINLINE vec2 Vector2MulScalar(vec2 vector, f32 scalar) {
    return (vec2)
    {vector.x * scalar, 
    vector.y * scalar};
}

/* 
 * @brief Method to divide two vec2s against one another
 * @param vecA The first vec2
 * @param vecB The second vec2
 * @return A vec2 of `vecA` divided by `vecB`
 */
VINLINE vec2 Vector2Div(vec2 vecA, vec2 vecB) {
    return (vec2)
    {vecA.x / vecB.x,
    vecA.y / vecB.y};
}

/* 
 * @brief Method to divide a vec2 by a given scalar
 * @param vector The vec2 to divide
 * @param scalar The scalar to divide against
 * @return The vec2 divided by the scalar
 */
VINLINE vec2 Vector2DivScalar(vec2 vector, f32 scalar) {
    return (vec2)
    {vector.x / scalar,
    vector.y / scalar};
}

/*
 * @brief Method to return the squared length of a vec2
 * @param vector The vector to find the length of
 * @return The squared length of the given vec2
 */
VINLINE f32 Vector2LengthSquared(vec2 vector) {
    return vector.x * vector.x + vector.y * vector.y;
}

/*
 * @brief Method to return the length of a vec2
 * @param vector The vec2 to find the length of
 * @return The length of the vec2
 */
VINLINE f32 Vector2Length(vec2 vector) {
    return vsqrt(Vector2LengthSquared(vector));
}

/* 
 * @brief Method to normalize a vec2
 * @param *vector A pointer to a vec2 to normalize
 */
VINLINE void Vector2Normalize(vec2* vector) {
    f32 length = Vector2Length(*vector);
    vector->x /= length;
    vector->y /= length;
}

/* 
 * @brief Method to give a normalized vec2
 * @param vector The vector to normalize
 * @return A normalized form of the given vec2
 */
VINLINE vec2 Vector2Normalized(vec2 vector) {
    Vector2Normalize(&vector);
    return vector;
}

/*
 * @brief Method to get the dot product of two vec2s
 * @param vecA The firest vec2
 * @param vecB The second vec2
 * @return A float of the difference between the two vec2s
 */
VINLINE f32 Vector2Dot(vec2 vecA, vec2 vecB) {
    f32 product = 0;
    product += vecA.x * vecB.x;
    product += vecA.y * vecB.y;
    return product;
}

/* 
 * @brief Method to find the angle in radians between two vec2s
 * @param vecA The first vec2
 * @param vecB The second vec2
 * @return An angle in radians between the two vec2s
 */
VINLINE f32 Vector2Angle(vec2 vecA, vec2 vecB) {
    f32 aLen = Vector2Length(vecA);
    f32 bLen = Vector2Length(vecB);
    // Assume vecB is the hypotenuse
    if (aLen > bLen) {
        return vacos(bLen / aLen);
    } else {
        return vacos(aLen / bLen);
    }
}


/*
 * ===================
 * VECTOR3 METHODS
 * ===================
 */


/* 
 * @brief Method to return a new vec3 with predetermined values
 * @param x The X value of the vector
 * @param y the Y value of the vector
 * @param z the Z value of the vector
 * @return A vec3 with the given values
 */
VINLINE vec3 Vector3(f32 x, f32 y, f32 z) {
    vec3 outVec;
    outVec.x = x;
    outVec.y = y;
    outVec.z = z;
    return outVec;
}

/* @brief Method to return a new vec3 with no values set */
VINLINE vec3 Vector3Zero() {
    return (vec3){0.0f, 0.0f, 0.0f};
}

VINLINE vec3 Vector3One() {
    return (vec3){1.0f, 1.0f, 1.0f};
}

/* @brief Method to return a vec3 pointing upwards */
VINLINE vec3 Vector3Up() {
    return (vec3){0.0f, 1.0f, 0.0f};
}

/* @brief Method to return a vec3 pointing downwards */
VINLINE vec3 Vector3Down() {
    return (vec3){0.0f, -1.0f, 0.0f};
}

/* @brief Method to return a vec3 pointing to the left */
VINLINE vec3 Vector3Left() {
    return (vec3){-1.0f, 0.0f, 0.0f};
}

/* @brief Method to return a vec3 pointing to the right */
VINLINE vec3 Vector3Right() {
    return (vec3){1.0f, 0.0f, 0.0f};
}

/* @brief Method to return a vec3 pointing forward into the screen */
VINLINE vec3 Vector3Forward() {
    return (vec3){0.0f, 0.0f, -1.0f};
}

/* @brief Method to return a vec3 pointing backwards out of the screen */
VINLINE vec3 Vector3Backward() {
    return (vec3){0.0f, 0.0f, 1.0f};
}

/* 
 * @brief Method to return the sum of two vec3s
 * @param vecA The first vector
 * @param vecB The second vector
 * @return A vec3 as the sum of both vectors
 */
VINLINE vec3 Vector3Add(vec3 vecA, vec3 vecB) {
    return (vec3)
    {vecA.x + vecB.x,
    vecA.y + vecB.y,
    vecA.z + vecB.z};
}

/*
 * @brief Method to subtract two vec3s from one another
 * @param vecA The first vector we are subtracting from
 * @param vecB The vector to subtract vecA from
 * @return A vec3 as the difference between the first and second vector
 */
VINLINE vec3 Vector3Sub(vec3 vecA, vec3 vecB) {
    return (vec3)
    {vecA.x - vecB.x,
    vecA.y - vecB.y,
    vecA.z - vecB.z};
}

/* 
 * @brief The product of two vec3s, as a general multiplication, NOT the dot or cross product.
 * @param vecA The first vec3
 * @param vecB The second vec3
 * @return The product of both vec3s
 */
VINLINE vec3 Vector3Mul(vec3 vecA, vec3 vecB) {
    return (vec3)
    {vecA.x * vecB.x,
    vecA.y * vecB.y,
    vecA.z * vecB.z};
}

/* 
 * @brief The product of a vec3 and a normal number
 * @param vecA The vec3 to multiply against
 * @param scalar The number we are multiplying the vector by
 * @return A vec3 multiplied by the scalar
 */
VINLINE vec3 Vector3MulScalar(vec3 vecA, f32 scalar) {
    return (vec3)
    {vecA.x * scalar,
    vecA.y * scalar,
    vecA.z * scalar};
}

/* 
 * @brief Method to divide two vec3s by one another
 * @param vecA The vec3 we are dividing against 
 * @param vecB The vec3 we are dividing vecA by
 * @return A vec3 divided by the two vectors
 */
VINLINE vec3 Vector3Div(vec3 vecA, vec3 vecB) {
    return (vec3)
    {vecA.x / vecB.x,
    vecA.y / vecB.y,
    vecA.z / vecB.z};
}

/* 
 * @brief Method to divide a vec3 by a given scalar
 * @param vecA The vector to divide by
 * @param scalar The scalar to divide the vec3 by
 * @return The result of the vector being divided by the scalar
 */
VINLINE vec3 Vector3DivScalar(vec3 vecA, f32 scalar) {
    return (vec3)
    {vecA.x / scalar,
    vecA.y / scalar,
    vecA.z / scalar};
}

/*
 * @brief Method to find the squared length of a vec3
 * @param vector The vec3  to square
 * @return The length squared of the vec3
 */
VINLINE f32 Vector3LengthSquared(vec3 vector) {
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

/*
 * @brief Method for finding the length of a vec3
 * @param vector The vec3 to find the length of
 * @returns A float of the length of the vec3
 */
VINLINE f32 Vector3Length(vec3 vector) {
    return vsqrt(Vector3LengthSquared(vector));
}

/* 
 * @brief Method to normalize a given vec3
 * @param *vector A pointer to the vec3 to normalize
 */
VINLINE void Vector3Normalize(vec3* vector) {
    f32 length = Vector3Length(*vector);
    vector->x /= length;
    vector->y /= length;
    vector->z /= length;
}

/*
 * @brief Method to normalize a vec3 and return it
 * @param vector The vec3 to normalize
 * @return A normalized version of `vector`
 */
VINLINE vec3 Vector3Normalized(vec3 vector) {
    Vector3Normalize(&vector);
    return vector;
}

/* 
 * @brief Method to get the dot product of two vectors, generally for direction purposes
 * @param vecA The first vec3
 * @param vecB The second vec3
 * @return A float of the difference between the two vec3s
 */
VINLINE f32 Vector3Dot(vec3 vecA, vec3 vecB) {
    f32 product = 0;
    product += vecA.x * vecB.x;
    product += vecA.y * vecB.y;
    product += vecA.z * vecB.z;
    return product;
}

/*
 * @brief Method to return the cross product of two vec3s
 * @param vecA The first vec3
 * @param vecB The second vec3
 * @return A vec3 orthogonal to both of the given vectors
 */
VINLINE vec3 Vector3Cross(vec3 vecA, vec3 vecB) {
    vec3 result;
    result.x = (vecA.y * vecB.z) - (vecA.z * vecB.y);
    result.y = (vecA.z * vecB.x) - (vecA.x * vecB.z);
    result.z = (vecA.x * vecB.y) - (vecA.y * vecB.x);
    return result;
}

/*
 * @brief Method to find the angle between two vectors 
 * @param vecA The first vec3
 * @param vecB The second vec3
 * @return A float representing the angl
 */
VINLINE f32 Vector3Angle(vec3 vecA, vec3 vecB) {
    f32 aLen = Vector3Length(vecA);
    f32 bLen = Vector3Length(vecB);
    // Assume vecB is the hypotenuse
    if (aLen > bLen) {
        return vacos(bLen / aLen);
    } else {
        return vacos(aLen / bLen);
    }
}


/*
 * ===================
 * VECTOR4 METHODS
 * ===================
 */


/* 
 * @brief Method for creating a vec4 with predetermined values
 * @param x The x value
 * @param y The y value
 * @param z The z value
 * @param w The w value
 * @return A vec4 with all the valus set
 */
VINLINE vec4 Vector4(f32 x, f32 y, f32 z, f32 w) {
    vec4 outVec;
    outVec.x = x;
    outVec.y = y;
    outVec.z = z;
    outVec.w = w;
    return outVec;
}

/* @brief Method to return a vec4 with all values set to 0 */
VINLINE vec4 Vector4Zero() {
    return (vec4){0.0f, 0.0f, 0.0f, 0.0f};
}

/* @brief Method to return a vec4 with all values set to 1*/
VINLINE vec4 Vector4One() {
    return (vec4){1.0f, 1.0f, 1.0f, 1.0f};
}

/*
 * @brief Method to convert a vec4 to a vec3
 * @param vector The vec4 to convert
 * @return A vec3
 */
VINLINE vec3 Vector4ToVector3(vec4 vector) {
    return (vec3){vector.x, vector.y, vector.z};
}

/* 
 * @brief Method to convert a vec3 into a vec4
 * @param vector The vec3 to convert
 * @param w The w component to add into the vec4
 * @return A vec4
 */
VINLINE vec4 Vector3ToVector4(vec3 vector, f32 w) {
    return (vec4){vector.x, vector.y, vector.z, w};
}

/* 
 * @brief Method to add two vec4s together
 * @param vecA The first vec4
 * @param vecB The second vec4
 * @return The sum of both vec4s
 */
VINLINE vec4 Vector4Add(vec4 vecA, vec4 vecB) {
    return (vec4)
    {vecA.x + vecB.x,
    vecA.y + vecB.y,
    vecA.z + vecB.z,
    vecA.w + vecB.w};
}

/* 
 * @brief Method to subtract one vec4 from another
 * @param vecA The vector to subtract
 * @param vecB The vector to subtract from 
 * @return The difference between the two vec4s
 */
VINLINE vec4 Vector4Sub(vec4 vecA, vec4 vecB) {
    return (vec4)
    {vecA.x - vecB.x,
    vecA.y - vecB.y,
    vecA.z - vecB.z,
    vecA.w - vecB.w};
}

/* 
 * @brief Method to multiply one vec4 against another
 * @param vecA The first vec4
 * @param vecB The second vec4
 * @return The product of both vec4s
 */
VINLINE vec4 Vector4Mul(vec4 vecA, vec4 vecB) {
    return (vec4)
    {vecA.x * vecB.x,
    vecA.y * vecB.y,
    vecA.z * vecB.z,
    vecA.w * vecB.w};
}

/* 
 * @brief Method to multiply a given vec4 by a scalar
 * @param vector The vector to multiply 
 * @param scalar The scalar to multiply the vec4 against
 * @return The product of the given vec4 and the scalar
 */
VINLINE vec4 Vector4MulScalar(vec4 vector, f32 scalar) {
    return (vec4)
    {vector.x * scalar,
    vector.y * scalar,
    vector.z * scalar,
    vector.w * scalar};
}

/* 
 * @brief Method to divide two vec4s by one another
 * @param vecA The first vec4
 * @param vecB The second vec4
 * @return The result of diving `vecA` by `vecB`
 */
VINLINE vec4 Vector4Div(vec4 vecA, vec4 vecB) {
    return (vec4)
    {vecA.x / vecB.x,
    vecA.y / vecB.y,
    vecA.z / vecB.z,
    vecA.w / vecB.w};
}

/*
 * @brief Method to divide a vec4 by a given scalar
 * @param vector The vec4 to divide
 * @param scalar The scalar to divide the vec4 against
 * @return The result of the vec4 being divided by the scalar
 */
VINLINE vec4 Vector4DivScalar(vec4 vector, f32 scalar) {
    return (vec4)
    {vector.x / scalar,
    vector.y / scalar,
    vector.z / scalar,
    vector.w / scalar};
}

/* 
 * @brief Method to find the squared length of a given vec4
 * @param vector The given vec4 to find the squared length of
 * @return The squared length of the vec4 
 */
VINLINE f32 Vector4LengthSquared(vec4 vector) {
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;
}

/* 
 * @brief Method to find the length of a given vec4
 * @param vector The given vec4 to find the length of
 * @return The length of the given vec4
 */
VINLINE f32 Vector4Length(vec4 vector) {
    return vsqrt(Vector4LengthSquared(vector));
}

/* 
 * @brief Method to normalize a vec4
 * @param *vector A pointer to the vec4 to normalize
 */
VINLINE void Vector4Normalize(vec4* vector) {
    f32 length = Vector4Length(*vector);
    vector->x /= length;
    vector->y /= length;
    vector->z /= length;
    vector->w /= length;
}

/*
 * @brief Method to return a normalized vec4
 * @param vector The vec4 to normalize
 * @return A normalized version of the given vec4
 */
VINLINE vec4 Vector4Normalized(vec4 vector) {
    Vector4Normalize(&vector);
    return vector;
}

/* 
 * @brief Method to give the dot product of two given vec4s
 * @param vecA The first vec4
 * @param vecB The second vec4
 * @return A float of the difference between the two vec4s
 */
VINLINE f32 Vector4Dot(vec4 vecA, vec4 vecB) {
    f32 product = 0;
    product += vecA.x * vecB.x;
    product += vecA.y * vecB.y;
    product += vecA.z * vecB.z;
    product += vecA.w * vecB.w;
    return product;
}

// Currently don't need anything other than 4x4 matrices, those aren't included

/*
 * ===================
 * 4x4 MATRIX METHODS
 * ===================
 */


/*
 * This is an identity matrix, which is needed in several functions in the program. We use a mat4 as we need the 4th column/row for modifying vec3s
 * {1, 0, 0, 0,
 *  0, 1, 0, 0,
 *  0, 0, 1, 0,
 *  0, 0, 0, 1};
 */

/* 
 * @brief Method to create a new `mat4` with all values set to `value`
 * @param value The value to set each part of the matrix to
 * @return A new `mat4`
 */
VINLINE mat4 Mat4New(f32 value) {
    mat4 outMat;
    outMat.data[0] = value;
    outMat.data[1] = value;
    outMat.data[2] = value;
    outMat.data[3] = value;
    outMat.data[4] = value;
    outMat.data[5] = value;
    outMat.data[6] = value;
    outMat.data[7] = value;
    outMat.data[8] = value;
    outMat.data[9] = value;
    outMat.data[10] = value;
    outMat.data[11] = value;
    outMat.data[12] = value;
    outMat.data[13] = value;
    outMat.data[14] = value;
    outMat.data[15] = value;
    return outMat;
}

/*
 * @brief Method to create a 4x4 identity matrix
 * @return Returns a 4x4 identity matrix
 */
VINLINE mat4 Mat4Identity() {
    mat4 outMat;
    VZero(outMat.data, sizeof(f32) * 16);
    outMat.data[0] = 1.0f;
    outMat.data[5] = 1.0f;
    outMat.data[10] = 1.0f;
    outMat.data[15] = 1.0f;
    return outMat;
}

/*
 * @brief Method to add two mat4s together
 * @param matA The first mat4
 * @param matB The second mat4
 * @return The sum of both `matA` and `matB`
 */
VINLINE mat4 Mat4Add(mat4 matA, mat4 matB) {
    return (mat4)
    {matA.data[0] + matB.data[0],
    matA.data[1] + matB.data[1],
    matA.data[2] + matB.data[2],
    matA.data[3] + matB.data[3],
    matA.data[4] + matB.data[4],
    matA.data[5] + matB.data[5],
    matA.data[6] + matB.data[6],
    matA.data[7] + matB.data[7],
    matA.data[8] + matB.data[8],
    matA.data[9] + matB.data[9],
    matA.data[10] + matB.data[10],
    matA.data[11] + matB.data[11],
    matA.data[12] + matB.data[12],
    matA.data[13] + matB.data[13],
    matA.data[14] + matB.data[14],
    matA.data[15] + matB.data[15]};
}

/*
 * @brief Method to subtract one mat4 by another
 * @param matA The first mat4
 * @param matB The second mat4
 * @return The difference between `matA` and `matB`
 */
VINLINE mat4 Mat4Sub(mat4 matA, mat4 matB) {
    return (mat4)
    {matA.data[0] - matB.data[0],
    matA.data[1] - matB.data[1],
    matA.data[2] - matB.data[2],
    matA.data[3] - matB.data[3],
    matA.data[4] - matB.data[4],
    matA.data[5] - matB.data[5],
    matA.data[6] - matB.data[6],
    matA.data[7] - matB.data[7],
    matA.data[8] - matB.data[8],
    matA.data[9] - matB.data[9],
    matA.data[10] - matB.data[10],
    matA.data[11] - matB.data[11],
    matA.data[12] - matB.data[12],
    matA.data[13] - matB.data[13],
    matA.data[14] - matB.data[14],
    matA.data[15] - matB.data[15]};
}

/*
 * @brief Method to get the sum of two mat4s
 * @param matA The first mat4
 * @param matB The second mat4
 * @return The sum of the two given mat4s
 */
VINLINE mat4 Mat4Mul(mat4 matA, mat4 matB) {
    mat4 outMat;
    outMat.data[0] = matA.data[0] * matB.data[0] + matA.data[1] * matB.data[4] + matA.data[2] * matB.data[8] + matA.data[3] * matB.data[12];
    outMat.data[1] = matA.data[0] * matB.data[1] + matA.data[1] * matB.data[5] + matA.data[2] * matB.data[9] + matA.data[3] * matB.data[13];
    outMat.data[2] = matA.data[0] * matB.data[2] + matA.data[1] * matB.data[6] + matA.data[2] * matB.data[10] + matA.data[3] * matB.data[14];
    outMat.data[3] = matA.data[0] * matB.data[3] + matA.data[1] * matB.data[7] + matA.data[2] * matB.data[11] + matA.data[3] * matB.data[15];
    outMat.data[4] = matA.data[4] * matB.data[0] + matA.data[5] * matB.data[4] + matA.data[6] * matB.data[8] + matA.data[7] * matB.data[12];
    outMat.data[5] = matA.data[4] * matB.data[1] + matA.data[5] * matB.data[5] + matA.data[6] * matB.data[9] + matA.data[7] * matB.data[13];
    outMat.data[6] = matA.data[4] * matB.data[2] + matA.data[5] * matB.data[6] + matA.data[6] * matB.data[10] + matA.data[7] * matB.data[14];
    outMat.data[7] = matA.data[4] * matB.data[3] + matA.data[5] * matB.data[7] + matA.data[6] * matB.data[11] + matA.data[7] * matB.data[15];
    outMat.data[8] = matA.data[8] * matB.data[0] + matA.data[9] * matB.data[4] + matA.data[10] * matB.data[8] + matA.data[11] * matB.data[12];
    outMat.data[9] = matA.data[8] * matB.data[1] + matA.data[9] * matB.data[5] + matA.data[10] * matB.data[9] + matA.data[11] * matB.data[13];
    outMat.data[10] = matA.data[8] * matB.data[2] + matA.data[9] * matB.data[6] + matA.data[10] * matB.data[10] + matA.data[11] * matB.data[14];
    outMat.data[11] = matA.data[8] * matB.data[3] + matA.data[9] * matB.data[7] + matA.data[10] * matB.data[11] + matA.data[11] * matB.data[15];
    outMat.data[12] = matA.data[12] * matB.data[0] + matA.data[13] * matB.data[4] + matA.data[14] * matB.data[8] + matA.data[15] * matB.data[12];
    outMat.data[13] = matA.data[12] * matB.data[1] + matA.data[13] * matB.data[5] + matA.data[14] * matB.data[9] + matA.data[15] * matB.data[13];
    outMat.data[14] = matA.data[12] * matB.data[2] + matA.data[13] * matB.data[6] + matA.data[14] * matB.data[10] + matA.data[15] * matB.data[14];
    outMat.data[15] = matA.data[12] * matB.data[3] + matA.data[13] * matB.data[7] + matA.data[14] * matB.data[11] + matA.data[15] * matB.data[15];
    return outMat;
}

// No current mat4 division, will be added if needed

/* 
 * @brief Method to multiply a mat4 by a vec4, often for translating/rotating/scaling the vec4
 * @param mat The mat4 to modify the vec4 by
 * @param vector The vec4 to modify
 * @return The result of the vec4 multiplied by the mat4
 */
VINLINE vec4 Mat4MulByVector4(mat4 mat, vec4 vector) {
    vec4 outVec;
    outVec.x = mat.data[0] * vector.x + mat.data[1] * vector.y + mat.data[2] * vector.z + mat.data[3] * vector.w;
    outVec.y = mat.data[4] * vector.x + mat.data[5] * vector.y + mat.data[6] * vector.z + mat.data[7] * vector.w;
    outVec.z = mat.data[8] * vector.x + mat.data[9] * vector.y + mat.data[10] * vector.z + mat.data[11] * vector.w;
    outVec.w = mat.data[12] * vector.x + mat.data[13] * vector.y + mat.data[14] * vector.z + mat.data[15] * vector.w;
    return outVec;
}

/*
 * @brief Method to translate a given vec4 by a given vec3
 * @param vectorA The vector to translate
 * @param vectorB The translation vector
 * @return A translated vec4
 */
VINLINE mat4 Mat4Translate(vec3 vector) {
    mat4 mulMat = Mat4Identity();
    mulMat.data[3] = vector.x;
    mulMat.data[7] = vector.y;
    mulMat.data[11] = vector.z;
    return mulMat;
}

/* 
 * @brief Method to give a rotation matrix around the x axis
 * @param theta The angle to rotate by in radians
 * @return The rotation matrix around the x axis for angle theta
 */
VINLINE mat4 Mat4EulerX(f32 theta) {
    mat4 rotMat = Mat4Identity();
    f32 sin = vsin(theta);
    f32 cos = vcos(theta);
    rotMat.data[5] = cos;
    rotMat.data[6] = -sin;
    rotMat.data[9] = sin;
    rotMat.data[10] = cos;
    return rotMat;
}

/* 
 * @brief Method to give a rotation matrix around the y axis
 * @param theta The angle to rotate by in radians
 * @return The rotation matrix around the y axis for angle theta
 */
VINLINE mat4 Mat4EulerY(f32 theta) {
    mat4 rotMat = Mat4Identity();
    f32 sin = vsin(theta);
    f32 cos = vcos(theta);
    rotMat.data[0] = cos;
    rotMat.data[2] = sin;
    rotMat.data[8] = -sin;
    rotMat.data[10] = cos;
    return rotMat;
}

/* 
 * @brief Method to give a rotation matrix around the z axis
 * @param theta The angle to rotate by in radians
 * @return The rotation matrix around the z axis for angle theta
 */
VINLINE mat4 Mat4EulerZ(f32 theta) {
    mat4 rotMat = Mat4Identity();
    f32 sin = vsin(theta);
    f32 cos = vcos(theta);
    rotMat.data[0] = cos;
    rotMat.data[1] = -sin;
    rotMat.data[4] = sin;
    rotMat.data[5] = cos;
    return rotMat;
}

/*
 * @brief Method to scale a vec4 by a given amount
 * @param vector The vector to scale
 * @param scale The scale vector
 * @return The scale matrix for the given scale vector
 */
VINLINE vec4 Mat4Scale(vec4 vector, vec3 scale) {
    mat4 outMat = Mat4Identity();
    outMat.data[0] = scale.x;
    outMat.data[5] = scale.y;
    outMat.data[10] = scale.z;
    return Mat4MulByVector4(outMat, vector);
}

/* 
 * @brief Method to get a mat4 multiplier for a given scale
 * @param scaleVec The scale vector
 * @return A `mat4` that when multiplied against with a `vec4` will scale it appropriately
 */
VINLINE mat4 Mat4ScaleMat(vec3 scaleVec) {
    mat4 outMat = Mat4Identity();
    outMat.data[0] = scaleVec.x;
    outMat.data[5] = scaleVec.y;
    outMat.data[10] = scaleVec.z;
    return outMat;
}

/*
 * @brief Method to generate an orthographic view matrix from a given set of values
 * @param top The upper screen limit of the matrix
 * @param bottom The lower screen limit of the matrix
 * @param left The left screen limit
 * @param right The right screen limit
 * @param near The near clipping plane
 * @param far The far clipping plane
 * @return A orthographic projection matrix valid for the given coordinates
 */
VINLINE mat4 Mat4Orthographic(f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far) {
    mat4 outMat = Mat4Identity();

    f32 rl = right - left;
    f32 tb = top - bottom;
    f32 fn = far - near;

    outMat.data[0] = 2.0f / rl;
    outMat.data[5] = 2.0f / tb;
    outMat.data[10] = -2.0f / fn;

    outMat.data[3] = -((right + left) / rl);
    outMat.data[7] = -((top + bottom) / tb);
    outMat.data[11] = -((far + near) / fn);
    return outMat;
}

/* 
 * @brief Method to generate a perspective view matrix from a given set of values
 * @param fov The field of view of the camera
 * @param aspectRatio The aspect ratio of the screen, i.e. `16:9` or `4:3`.
 * @param near The near clipping plane
 * @param far The far clipping plane
 */
VINLINE mat4 Mat4Perspective(f32 fov, f32 aspectRatio, f32 near, f32 far) {
    f32 halfTanFov = vtan(fov * 0.5f);

    mat4 outMat;
    VZero(outMat.data, sizeof(f32) * 16);

    outMat.data[0] = 1.0f / (aspectRatio * halfTanFov);
    outMat.data[5] = 1.0f / halfTanFov;
    outMat.data[10] = -((far + near) / (far - near));
    outMat.data[11] = -1.0f;
    outMat.data[14] = -((2.0f * far * near) / (far - near));
    return outMat;
}

