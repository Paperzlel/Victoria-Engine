#pragma once

#include "defines.h"
#include "core/logger.h"

#include "opengl-types.inl"
#include "renderer/renderer-types.inl"


b8 GLBackendInitialize(RendererBackend* backend, i32 width, i32 height, string applicationName);

i8 GLBackendRenderFrame(RendererBackend* backend);

void GLBackendShutdownRendering(RendererBackend* backend);

void GLBackendFramebufferSizeCallback(GLFWwindow* window, i32 width, i32 height);

//TODO: Better input mapping, this is temporary.
void GLProcessInput(GLFWwindow* window);