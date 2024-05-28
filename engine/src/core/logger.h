#pragma once

#include "defines.h"

typedef enum LogLevel {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN = 2,
    LOG_LEVEL_DEBUG = 3,
    LOG_LEVEL_INFO = 4,
    LOG_LEVEL_TRACE = 5
} LogLevel;

//TODO: Create methods once platform-agnostic
b8 InitialiseLogging();
void ShutdownLogging();

VAPI void LogMessage(LogLevel level, string message, ...);

/* Logging defines. 
 * We call these macros to avoid calling LogMessage a bunch
 * TODO: Add configurable logging (i.e. release doesn't output debug and so forth)
 */

#define VFATAL(message, ...) LogMessage(LOG_LEVEL_FATAL,message, ##__VA_ARGS__);

#define VERROR(message, ...) LogMessage(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);

#define VWARN(message, ...) LogMessage(LOG_LEVEL_WARN, message, ##__VA_ARGS__);

#define VDEBUG(message, ...) LogMessage(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);

#define VINFO(message, ...) LogMessage(LOG_LEVEL_INFO, message, ##__VA_ARGS__);

#define VTRACE(message, ...) LogMessage(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);

