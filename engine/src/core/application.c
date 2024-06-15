#include "application.h"

b8 ApplicationInitialize(ApplicationState* state, RendererObject* renderObj) {
    if (state->isInitialized) {
        VERROR("Application is already open.");
        return FALSE;
    }

    // Add other initialize functions here

    if (!FrontendInitializeRendering(renderObj)) {
        VFATAL("Frontend failed to initialize.");
        return FALSE;
    }

    state->deltaTime = 0.0f;
    state->isInitialized = TRUE;
    VINFO("Application has initialized.");
    return TRUE;
}