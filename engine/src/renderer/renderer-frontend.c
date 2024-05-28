#include "renderer-frontend.h"

//TODO: Add a method to switch renderers.

b8 FrontendInitializeRendering(RendererObject* object) {
    if (object->isActive) {
        VERROR("OpenGL is already active!");
        return FALSE;
    }

    if (!(object->type == RENDERER_OPENGL)) {
        VFATAL("OpenGL not selected! Please choose a valid renderer.");
        return FALSE;
    }
    if (!BackendInitializeGLFW(&object->context, 800, 600)) {
        VFATAL("GLFW failed to initialize!");
        return FALSE;
    }
    object->isActive = TRUE;
    return TRUE;
}

b8 FrontendRenderFrame(RendererObject* object) {
    if (!(object->type == RENDERER_OPENGL)) {
        VFATAL("OpenGL is not being used!");
        return FALSE;
    }
    //TODO: Hook up object context
    if (!BackendRenderFrame(object->context.window, 800, 600)) {
        VFATAL("Backend could not render the frame!");
        return FALSE;
    }
    return TRUE;
}

void FrontendShutdownRendering(RendererObject* object) {
    BackendShutdownRendering(object->context.window);
}