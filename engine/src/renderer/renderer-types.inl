#pragma once

#include "defines.h"

/* File to hold all the structs relating to rendering */

typedef enum RendererBackendType {
    RENDERER_OPENGL = 0,
    RENDERER_VULKAN = 1
    // etc.
} RendererBackendType;


typedef struct RendererBackend {
    b8 (*initialize)(struct RendererBackend* backend, i32 width, i32 height, string applicationName);
    void (*shutdown)(struct RendererBackend* backend);
    i8 (*drawFrame)(struct RendererBackend* backend);
} RendererBackend;