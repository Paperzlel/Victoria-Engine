#pragma once

#include "defines.h"
#include "core/logger.h"
#include "open-gl/opengl-backend.h"

#define RENDERER_NONE -1
#define RENDERER_OPENGL 0
#define RENDERER_VULKAN 1
//Add other types if needed

typedef struct RendererObject {
    u8 type;
    b8 isActive;
    openGLContext context;
} RendererObject;


VAPI b8 FrontendInitializeRendering(RendererObject* object);

VAPI b8 FrontendRenderFrame(RendererObject* object);

VAPI void FrontendShutdownRendering(RendererObject* object);