#pragma once

#include "defines.h"

//TODO: Change string header file to a darray-types header file
#include "core/vstring.h"

/* File for containing all the structures we use across our OpenGL code. */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* The type of draw mode for a loaded object. */
typedef enum DrawType {
    STREAM_DRAW = GL_STREAM_DRAW,
    STATIC_DRAW = GL_STATIC_DRAW,
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW
} DrawType;

typedef enum ColourRange {
    RGB = GL_RGB,
    RGBA = GL_RGBA
} ColourRange;

typedef struct Shader {
    // Unique shader ID TODO: Add functionality
    u32 ID;
    // Name of the shader, passed down from any materials loaded
    string name;
    // The fragment shader 
    u32 fragmentShader;
    // The vertex shader
    u32 vertexShader;
    // The contents of both shaders
    string contents[2];
    // Whether a uniform was detected
    b8 hasUniform;
    //Array of listed uniforms
    StringArray uniforms;
    //Array of listed uniform types
    StringArray uniformTypes;
} Shader;

typedef struct Texture {
    u32 id;
    ColourRange range;
    i32 texWidth;
    i32 texHeight;
    i32 channelCount;
    u8* data;
    u32 texID;
} Texture;

/* The context for OpenGL, containing all the universal data we need */
typedef struct OpenGLContext {
    GLFWwindow* window;
    u32 shaderProgram;
    u32 vertexBufferObj;
    u32 vertexArrayObj;
    u32 elementBufferObj;
    //TODO: Move to separate program
    Texture texture;
    f32 deltaTime;
    b8 windowShouldClose;
} OpenGLContext;
