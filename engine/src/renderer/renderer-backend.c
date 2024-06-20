#include "renderer-backend.h"
#include "open-gl/opengl-backend.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

b8 CreateRendererBackend(RendererBackendType type, RendererBackend* backend) {
    if (type == RENDERER_OPENGL) {
        backend->initialize = GLBackendInitialize;
        backend->drawFrame = GLBackendRenderFrame;
        backend->shutdown = GLBackendShutdownRendering;

        return TRUE;
    }

    return FALSE;
}

void DestroyRendererBackend(RendererBackend* backend) {
    backend->initialize = 0;
    backend->shutdown = 0;
    backend->drawFrame = 0;
}