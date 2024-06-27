#pragma once

#include "defines.h"

#include "containers/array.h"

#define MAX_STRING_COUNT 512

/* 
 * @brief A method to give the number of characters in a string
 * @param in The string that you want to input 
 */
VAPI i32 StringLength(string in);

/* 
 * @brief A method to compare one string to another
 * @param str The "main" string, a.k.a the string you want to compare with
 * @param comparisonStr The string you compare something else with
 * @param *outPos [OPTIONAL] The position of the `comparisonStr` in the string
 * @return TRUE if comparisonStr is in str, FALSE if not
 */
VAPI b8 StringContains(string str, string comparisonStr, u32* outPos);

/*
 * @brief A method to format one string with another
 * @param dest The destination string we are formatting
 * @param in The string that we want to format into the main string
 * @param ... Variatic arguments, if the string we want to format into has numbers/other strings, add them in order here
 */
VAPI void FormatString(string* dest, string in, ...);

/* 
 * @brief Method to split a string into multiple different parts, based on a delimiter
 * @param dest The string we are inputting into the file to be split
 * @param delimiter The split that the string will look for
 * @param *array A pointer to a StringArray that will hold all of the individual parts
 * @return `TRUE` if the string was able to be split, `FALSE` if there was no matching `splitType` in the string
 */
VAPI b8 StringSplit(string in, string delimiter, StringArray* array);

