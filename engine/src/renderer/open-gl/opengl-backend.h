#pragma once

#include "defines.h"
#include "core/logger.h"

#include "include/glad.h"
#include "include/glfw3.h"

typedef struct openGLContext {
    GLFWwindow* window;
} openGLContext;

b8 BackendInitializeGLFW(openGLContext* context, i32 width, i32 height);

b8 BackendRenderFrame(GLFWwindow* window, i32 width, i32 height);

b8 BackendShutdownRendering(GLFWwindow* window);

GLFWwindow* GetCurrentWindow();
