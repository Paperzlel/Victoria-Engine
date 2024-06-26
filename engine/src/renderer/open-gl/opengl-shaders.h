#pragma once

#include "defines.h"

#include "opengl-types.inl"

typedef enum ShaderType {
    FRAGMENT_SHADER = 0,
    VERTEX_SHADER = 1,
    SHADER_PROGRAM = 2
} ShaderType;

char* GLGetShaderContent(Shader* shader, ShaderType type);

b8 GLCompileShader(Shader* shader, ShaderType type, u8 index);

void GLAttachShaderToProgram(u32* shaderProgram, Shader* shader, ShaderType type);

b8 GLListAllUniforms(Shader* shader, u8 index);

b8 GLUseUniform(string uniformType, string uniformName);

b8 GLRegisterNewShader(Shader* shader, u32* shaderProgram);

void GLDeleteShaders(Shader* shader);

b8 GLInitializeShaderProgram(u32* shaderProgram);

b8 GLLinkShaderProgram(u32* shaderProgram);

b8 GLShaderCheckCompileProblems(u32* loc, ShaderType type);