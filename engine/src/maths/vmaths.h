#pragma once

#include "defines.h"

#include "maths-types.h"

/* 
 * ===================
 * VECTOR2 METHODS
 * ===================
 */

/* @brief Method to return a new vec2 with predetermined values */
VINLINE vec2 Vector2(f32 x, f32 y);

/* @brief Method to return a new Vector2 with no values set */
VINLINE vec2 Vector2Zero();

/*
 * ===================
 * TRIGONOMETRY METHODS
 * ===================
 */

/* @brief Method for a sine wave.
 * @param value The x position along the tan wave
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
