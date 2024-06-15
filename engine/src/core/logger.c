#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>


b8 InitialiseLogging() {
    VINFO("Logging initialized successfully.");
    return TRUE;
}

void ShutdownLogging() {
    VINFO("Logging shutdown successfully.");
}

void LogMessage(LogLevel level, string message, ...) {
    string levelStrings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[DEBUG]: ", "[INFO]: ", "[TRACE]: "};

    // The message we want to send out to the console
    char outMessage[MAX_LOG_ENTRY];
    memset(outMessage, 0, sizeof(outMessage));

    // Handling variatic arguments for the message, a.k.a formatting variables into the string
    
    __builtin_va_list argPtr;
    va_start(argPtr, message);
    vsnprintf(outMessage, MAX_LOG_ENTRY, message, argPtr);
    va_end(argPtr);

    char secondOutMessage[MAX_LOG_ENTRY];
    sprintf(secondOutMessage, "%s%s\n", levelStrings[level], outMessage);

    printf("%s", secondOutMessage);
}