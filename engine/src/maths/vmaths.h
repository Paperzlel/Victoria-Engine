#pragma once

#include "defines.h"

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
