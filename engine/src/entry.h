#pragma once

#include "core/logger.h"
#include "core/application.h"
#include "core/vmemory.h"

#include "renderer/renderer-frontend.h"

static ApplicationState appState;

int main(void) {
    if (!ApplicationInitialize(&appState)) {
        VFATAL("Application failed to initialize.");
        return -1;
    }

    if (!ApplicationRun(&appState)) {
        VINFO("Application did not shutdown as expected due to an error in the application code.");
        return 2;
    }

    ApplicationShutdown(&appState);

    return 0;
} 