#pragma once

#include "defines.h"
#include "core/logger.h"
#include "maths/vmaths.h"

#include "opengl-types.inl"


b8 BackendInitializeGLFW(OpenGLContext* context, i32 width, i32 height);

b8 BackendRenderFrame(OpenGLContext* context, i32 width, i32 height);

b8 BackendShutdownRendering(OpenGLContext* context);

void BackendFramebufferSizeCallback(GLFWwindow* window, i32 width, i32 height);

//TODO: Better input mapping, this is temporary.
void ProcessInput(GLFWwindow* window);