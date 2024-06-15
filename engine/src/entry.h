#pragma once

#include "core/logger.h"
#include "core/application.h"
#include "core/vmemory.h"

#include "renderer/renderer-frontend.h"

static ApplicationState appState;
static RendererObject renderObj;

int main(void) {
    renderObj.type = RENDERER_OPENGL;

    if (!ApplicationInitialize(&appState, &renderObj)) {
        VFATAL("Application failed to initialize.");
        return -1;
    }

    while (!renderObj.context.windowShouldClose) {
        if (!FrontendRenderFrame(&renderObj)) {
            renderObj.isActive = FALSE;
            break;
        }
        LogMemoryUsage();
    }
    FrontendShutdownRendering(&renderObj);
    return 0;
} 