#pragma once

#include "defines.h"

/* 
 * @brief A method to give the number of characters in a string
 * @param in The string that you want to input 
 */
VAPI i32 StringLength(string in);

/* 
 * @brief A method to compare one string to another
 * @param str The "main" string, a.k.a the string you want to compare with
 * @param comparisonStr The string you compare something else with
 * @return TRUE if comparisonStr is in str, FALSE if not
 */
VAPI b8 StringContains(string str, string comparisonStr);

/*
 * @brief A method to format one string with another
 * @param dest The destination string we are formatting
 * @param in The string that we want to format into the main string
 * @param ... Variatic arguments, if the string we want to format into has numbers/other strings, add them in order here
 */
VAPI void FormatString(string* dest, string in, ...);
