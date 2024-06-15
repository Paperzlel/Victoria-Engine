#pragma once

#include "defines.h"
#include "core/logger.h"

#include "open-gl/opengl-backend.h"
#include "renderer-types.inl"

#define RENDERER_NONE -1
#define RENDERER_OPENGL 0
#define RENDERER_VULKAN 1
//Add other types if needed


VAPI b8 FrontendInitializeRendering(RendererObject* object);

VAPI b8 FrontendRenderFrame(RendererObject* object);

VAPI void FrontendShutdownRendering(RendererObject* object);

VAPI f32 GetDeltaTime(RendererObject* object);