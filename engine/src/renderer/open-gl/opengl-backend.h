#pragma once

#include "defines.h"
#include "core/logger.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct openGLContext {
    GLFWwindow* window;
    //TODO: move to separate struct
    u32 shaderProgram;
    u32 VAO;
    u32 VBO;
    u32 EBO;
} openGLContext;

b8 BackendInitializeGLFW(openGLContext* context, i32 width, i32 height);

b8 BackendRenderFrame(openGLContext* context, i32 width, i32 height);

b8 BackendShutdownRendering(openGLContext* context);

void BackendFramebufferSizeCallback(GLFWwindow* window, i32 width, i32 height);