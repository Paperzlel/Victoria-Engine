#pragma once

#include "defines.h"
#include "logger.h"
#include "renderer/renderer-frontend.h"

typedef struct ApplicationState {
    b8 isInitialized;
    f32 deltaTime;
} ApplicationState;

/*
 * @brief The method that initializes the application
 * @param state A pointer to the current application state
 * @param renderObj A pointer to the current rendering object
 */
VAPI b8 ApplicationInitialize(ApplicationState* state, RendererObject* renderObj);

