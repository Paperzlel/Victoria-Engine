#pragma once

#include "defines.h"

/* File for containing all the structures we use across our OpenGL code. */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* The type of draw mode for a loaded object. */
typedef enum DrawType {
    STREAM_DRAW = GL_STREAM_DRAW,
    STATIC_DRAW = GL_STATIC_DRAW,
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW
} DrawType;

/* The context for OpenGL, containing all the universal data we need */
typedef struct OpenGLContext {
    GLFWwindow* window;
    u32 shaderProgram;
    u32 vertexBufferObj;
    u32 vertexArrayObj;
    f32 deltaTime;
    b8 windowShouldClose;
} OpenGLContext;

typedef struct Shader {
    u32 ID;
    u32 vertexShader;
    u32 fragmentShader;
    string contents[2];
    string paths[2];
    string name; // Passed down from the texture
    b8 hasUniform;
} Shader;
