#pragma once

#include "defines.h"

#include "renderer-types.inl"

b8 CreateRendererBackend(RendererBackendType type, RendererBackend* backend);

void DestroyRendererBackend(RendererBackend* backend);