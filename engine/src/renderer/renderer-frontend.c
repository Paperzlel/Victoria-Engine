#include "renderer-frontend.h"

#include "renderer-backend.h"

#include "core/logger.h"
#include "core/vmemory.h"

static RendererBackend* backend = 0;

b8 FrontendInitializeRendering(string applicationName) {
    backend = VAllocate(sizeof(RendererBackend), MEMORY_TAG_RENDERER);

    CreateRendererBackend(RENDERER_OPENGL, backend);

    //TODO: Custom window sizing
    if (!backend->initialize(backend, 800, 600, applicationName)) {
        VFATAL("Rendering backend failed to initialize.");
        return FALSE;
    }

    return TRUE;
}

void FrontendShutdownRendering() {
    backend->shutdown(backend);
    VFree(backend, sizeof(backend), MEMORY_TAG_RENDERER);
}

i8 FrontendRenderFrame() {
    return backend->drawFrame(backend);
}