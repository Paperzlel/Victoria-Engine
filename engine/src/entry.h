#pragma once

#include "core/logger.h"
#include "renderer/renderer-frontend.h"

static RendererObject object;

//Run main here instead of in the .c file in testbed so that we don't get linker errors
int main(void) {
    VDEBUG("Here's a message that this works too!");
    object.isActive = FALSE;
    object.type = RENDERER_OPENGL;

    if (!FrontendInitializeRendering(&object)) {
        VFATAL("Frontend could not begin to render!");
        return -1;
    }
    while (object.isActive) {
        if (!FrontendRenderFrame(&object)) {
            object.isActive = FALSE;
            break;
        }
    }
    FrontendShutdownRendering(&object);
    return 0;
} 