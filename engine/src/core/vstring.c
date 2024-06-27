#include "vstring.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "logger.h"
#include "vmemory.h"


i32 StringLength(string in) {
    i32 len = 0;
    while (in[len] != '\0') {
        len++;
    }
    return len;
}

b8 StringContains(string str, string comparisonStr, u32* outPos) {
    // Get length of the two strings
    i32 compLen = StringLength(comparisonStr);
    i32 mainLen = StringLength(str);

    // Check if the comparison string is larger than the main string
    if (mainLen < compLen) {
        VWARN("String to compare with is larger than the string we are comparing. Please ensure the comparison string is shorter than the string to compare with.");
        return FALSE;
    }

    // Offset measures how far from the beginning we are in the string, validValues measures how many characters in the string exist
    i32 offset = 0;
    i32 validValues = 0;
    i32 compOffset = 0;
    while (offset < mainLen) {
        if (str[offset] == comparisonStr[compOffset]) {
            validValues++;
            compOffset++;
        } else {
            validValues = 0;
            compOffset = 0;
        }

        if (validValues + 1 == compLen) {
            *outPos = offset;
            return TRUE;
        }
        offset++;
    }
    return FALSE;
}

void FormatString(string* dest, string in, ...) {
    char outMessage[MAX_LOG_ENTRY];

    __builtin_va_list argPtr;
    va_start(argPtr, in);
    vsnprintf(outMessage, MAX_LOG_ENTRY, in, argPtr);
    va_end(argPtr);

    char outMessage2[MAX_LOG_ENTRY];
    sprintf(outMessage2, "%s", outMessage);
    
    *dest = outMessage2;
}

b8 StringSplit(string in, string delimiter, StringArray* array) {
    // Check length of the split string
    u32 splitLen = StringLength(delimiter);
    // Current index in the loop
    u32 index = 0;
    // Index for i (need it in a loop)
    u32 iTotal = 0;
    // Number of elements between index and the previous split
    u16 elements = 0;
    // Copy of the index to use with the string array
    u16 copyIndex = 0;
    // Similarity of the string to the split type
    u8 similarity = 0;
    // The current index into the array
    u8 currentArrIndex = 0;
    // An array of char* (string is immutable) that represent each split
    char* stringArr[MAX_STRING_COUNT] = {0};
    // A copy of the destination string to allow for looping
    char* stringcpy = (char*)in;

    // Main loop
    while (stringcpy[index] != '\0') {
        // TODO: >1 length split
        if (stringcpy[index] == delimiter[similarity]) {
            similarity++;
            if (similarity == splitLen) {
                // Check for if this is the first element in the array
                if (elements > 0) {
                    iTotal = elements + splitLen;
                }

                if (array->elementCount < currentArrIndex) {
                    ResizeStringArray(array, array->size * 2);
                }

                stringArr[currentArrIndex] = VAllocate(sizeof(char) * (index - elements), MEMORY_TAG_ARRAY);
                stringArr[currentArrIndex] = VZero(stringArr[currentArrIndex], sizeof(char) * (index - elements));
                for (i32 i = iTotal; i < index; i++) {
                    stringArr[currentArrIndex][copyIndex] = stringcpy[i];
                    copyIndex++;
                }
                stringArr[currentArrIndex][copyIndex] = '\0'; 
                elements = index;
                similarity = 0;
                copyIndex = 0;
                currentArrIndex++;
            }
        }
        index++;
    }
    // Last check out of the loop, if index is still 0 then no splits were detected, otherwise split once more
    if (currentArrIndex == 0) {
        VWARN("String could not be split.");
        return FALSE;
    } else {
        iTotal = elements + splitLen;
        stringArr[currentArrIndex] = VAllocate(sizeof(char) * (index - elements), MEMORY_TAG_ARRAY);
        stringArr[currentArrIndex] = VZero(stringArr[currentArrIndex], sizeof(char) * (index - elements));
        for (i32 i = iTotal; i < index; i++) {
            stringArr[currentArrIndex][copyIndex] = stringcpy[i];
            copyIndex++;
        }
        copyIndex = 0;
    }

    // TODO: Make my own file to do this in
    array->elementCount = currentArrIndex + 1;
    array->contents = VAllocate(array->elementCount * sizeof(char*), MEMORY_TAG_ARRAY);
    for (i32 i = 0; i < array->elementCount; i++) {
        array->contents[i] = stringArr[i];
    }

    return TRUE;
}

