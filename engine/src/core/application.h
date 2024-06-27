#pragma once

#include "defines.h"

typedef struct ApplicationState {
    b8 isInitialized;
    b8 isRunning;
    f32 deltaTime;
} ApplicationState;

/*
 * @brief The method that initializes the application
 * @param state A pointer to the current application state
 * @param renderObj A pointer to the current rendering object
 */
VAPI b8 ApplicationInitialize(ApplicationState* state);

VAPI b8 ApplicationRun(ApplicationState* state);

VAPI void ApplicationShutdown();