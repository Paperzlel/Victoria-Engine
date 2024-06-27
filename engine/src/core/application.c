#include "application.h"

#include "renderer/renderer-frontend.h"

#include "logger.h"
#include "vmemory.h"
#include "vstring.h"

#include "maths/vmaths.h"

b8 ApplicationInitialize(ApplicationState* state) {
    if (state->isInitialized) {
        VERROR("Application is already open.");
        return FALSE;
    }

    // Add other initialize functions here

    if (!FrontendInitializeRendering("Victoria Engine")) {
        VFATAL("Frontend failed to initialize.");
        return FALSE;
    }

    state->deltaTime = 0.0f;
    state->isInitialized = TRUE;
    state->isRunning = TRUE;
    VINFO("Application has initialized.");
    return TRUE;
}

b8 ApplicationRun(ApplicationState* state) {
    // MAIN LOOP
    while(state->isRunning) {
        // If memory values change, notify the user.
        LogMemoryUsage();
        // Results are a set of valid integers that release based on whether the frontend has properly stopped running,
        // or if the application has encountered a fatal error and needs to close.
        i8 result = FrontendRenderFrame();
        if (result < 0 || result > 2) {
            // Fatal error
            VFATAL("Frontend could not render the frame. Booting out...");
            return FALSE;
        } else if (!result) {
            // Frontend booted out properly. No cause for concern.
            VDEBUG("Application booted out successfully.");
            state->isRunning = FALSE;
            break;
        }
    }

    // Application exited successfully
    return TRUE;
}

void ApplicationShutdown(ApplicationState* state) {
    state = 0;
    VINFO("Application shut down successfully.");
}