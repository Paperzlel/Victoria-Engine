#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

b8 InitialiseLogging() {
    return TRUE;
}

void ShutdownLogging() {
    printf("lol");
}

void LogMessage(LogLevel level, string message, ...) {
    string levelStrings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[DEBUG]: ", "[INFO]: ", "[TRACE]: "};
    //b8 isError = level < 2;
    
    // The message we want to send out to the console
    char outMessage[32000];
    memset(outMessage, 0, sizeof(outMessage));

    // Handling variatic arguments for the message, a.k.a formatting variables into strings
    __builtin_va_list argPtr;
    va_start(argPtr, message);
    vsnprintf(outMessage, 32000, message, argPtr);
    va_end(argPtr);

    char secondOutMessage[32000];
    sprintf(secondOutMessage, "%s%s\n", levelStrings[level], outMessage);

    printf("%s", secondOutMessage);
}