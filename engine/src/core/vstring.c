#include "vstring.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "logger.h"

i32 StringLength(string in) {
    i32 len = 0;
    while (in[len] != '\0') {
        len++;
    }
    return len;
}

b8 StringContains(string str, string comparisonStr) {
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
