#include "renderer-frontend.h"

//TODO: Add a method to switch renderers.

b8 FrontendInitializeRendering(RendererObject* object) {
    if (object->isActive) {
        VERROR("Renderer is already active!");
        return FALSE;
    }

    if (object->type != RENDERER_OPENGL) {
        VFATAL("OpenGL not selected! Please choose a valid renderer.");
        return FALSE;
    }
    if (!BackendInitializeGLFW(&object->context, 800, 600)) {
        VFATAL("GLFW failed to initialize!");
        return FALSE;
    }
    object->isActive = TRUE;
    VINFO("Renderer initialized.");
    return TRUE;
}

b8 FrontendRenderFrame(RendererObject* object) {
    if (object->type != RENDERER_OPENGL) {
        VFATAL("OpenGL is not being used!");
        return FALSE;
    }

    if (!BackendRenderFrame(&object->context, 800, 600)) {
        VFATAL("Backend failed to render the frame!");
        return FALSE;
    }
    return TRUE;
}

void FrontendShutdownRendering(RendererObject* object) {
    BackendShutdownRendering(&object->context);
}

f32 GetDeltaTime(RendererObject* object) {
    return object->context.deltaTime;
}