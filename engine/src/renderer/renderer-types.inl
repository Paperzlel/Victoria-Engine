#pragma once

#include "defines.h"

#include "open-gl/opengl-types.inl"

/* File to hold all the structs relating to rendering */

typedef enum RendererBackendType {
    RENDERER_OPENGL = 0,
    RENDERER_VULKAN = 1
    // etc.
} RendererBackendType;

typedef struct RendererObject {
    u8 type;
    b8 isActive;
    OpenGLContext context;
} RendererObject;