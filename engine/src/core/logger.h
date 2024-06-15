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
/* 
 * @brief The method we call to begin logging
 * @return TRUE if it succeeded, FALSE if it did not.
 */
b8 InitialiseLogging();

/* 
 * @brief The method to shutdown our logging system.
 */
void ShutdownLogging();

/*
 * @brief Method to log a message to our console. Please use the macros (i.e. VINFO) instead of this method to save manually typing out each command
 * @param level The significance/danger the error has for our engine
 * @param message The message we want to send to the console
 * @param ... Optional extra arguments for the message parameter
 */
VAPI void LogMessage(LogLevel level, string message, ...);

/* Logging defines. 
 * We call these macros to avoid calling LogMessage a bunch
 * TODO: Add configurable logging (i.e. release doesn't output debug and so forth)
 */

#define VFATAL(message, ...) LogMessage(LOG_LEVEL_FATAL, message, ##__VA_ARGS__);

#define VERROR(message, ...) LogMessage(LOG_LEVEL_ERROR, message, ##__VA_ARGS__);

#define VWARN(message, ...) LogMessage(LOG_LEVEL_WARN, message, ##__VA_ARGS__);

#define VDEBUG(message, ...) LogMessage(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__);

#define VINFO(message, ...) LogMessage(LOG_LEVEL_INFO, message, ##__VA_ARGS__);

#define VTRACE(message, ...) LogMessage(LOG_LEVEL_TRACE, message, ##__VA_ARGS__);

