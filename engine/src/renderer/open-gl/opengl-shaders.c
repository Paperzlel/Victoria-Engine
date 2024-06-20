#include "opengl-shaders.h"

#include "core/logger.h"
#include "core/vmemory.h"
#include "core/vstring.h"

#include <glad/glad.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* File for handling OpenGL shader initialisation and destruction.
 * 
 * Important NOTE: We must create COPIES of a shader program and 
 * pass their reference instead of setting them as a pointer. This
 * is to avoid passing a double reference, and we have a lot of
 * shaders to use, anyway.
 */

// NOTE: We could load the layout data through parsing the number of location values there are
char* GLGetShaderContent(Shader* shader, ShaderType type) {
    char* shaderPart;
    if (type == FRAGMENT_SHADER) {
        shaderPart = "fs";
    }
    if (type == VERTEX_SHADER) {
        shaderPart= "vs";
    }

    //TODO: Make general engine calls to find CWD and set folder locations.
    char subfolders[512] = "assets/";
    string totalFilePath = "C:/Programs/C-and-C++/Victoria-Engine/";

    //Format the total file path to point directly to the file we want to load
    FormatString(&totalFilePath, "%s%s%s.%s", totalFilePath, subfolders, shader->name, shaderPart);

    FILE* fptr = 0;
    f64 size = 0;
    char* shaderFileContent;

    // NOTE: reading from totalFilePath is temporary as I haven't yet got a way to load textures in a custom manner.

    /* Read file to get size */
    fptr = fopen(totalFilePath, "rb");
    if (fptr == NULL) {
        return "";
    }
    fseek(fptr, 0L, SEEK_END);
    size = ftell(fptr)+1;
    fclose(fptr);


    fptr = fopen(totalFilePath, "r");
    
    // Memory allocation and freeing
    void* allocSize = VAllocate(size, MEMORY_TAG_FILE);
    shaderFileContent = VZero(allocSize, size, MEMORY_TAG_APPLICATION);
    fread(shaderFileContent, 1, size-1, fptr);
    fclose(fptr);
    //TODO: Currently freeing memory here messes up the file contents. We need to find a way to free the filepointer memory afterwards
    return shaderFileContent;
}

b8 GLCompileShader(Shader* shader, ShaderType type, u8 index) {
    if (!shader) {
        VERROR("Shader is null, cannot compile.");
        return FALSE;
    }

    if (type == VERTEX_SHADER) {
        shader->vertexShader = glCreateShader(GL_VERTEX_SHADER);

        glShaderSource(shader->vertexShader, 1, &shader->contents[index], NULL);
        glCompileShader(shader->vertexShader);

        if (!GLShaderCheckCompileProblems(&shader->vertexShader, type)) {
            return FALSE;
        }
    } else if (type == FRAGMENT_SHADER) {
        shader->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(shader->fragmentShader, 1, &shader->contents[index], NULL);
        glCompileShader(shader->fragmentShader);

        if (!GLShaderCheckCompileProblems(&shader->fragmentShader, type)) {
            return FALSE;
        }
    } else {
        VERROR("Unsupported shader type!");
        return FALSE;
    }
    return TRUE;
}

void GLAttachShaderToProgram(u32* shaderProgram, Shader* shader, ShaderType type) {
    if (type == VERTEX_SHADER) {
        glAttachShader(*shaderProgram, shader->vertexShader);
    }
    if (type == FRAGMENT_SHADER) {
        glAttachShader(*shaderProgram, shader->fragmentShader);
    }
}  

b8 GLRegisterNewShader(Shader* shader, u32* shaderProgram) {
    //TODO: Add functions to find length of an array
    i32 noOfElements = 2;

    // Loop over each iteration of the vertex/fragment shader
    for (i32 i = 0; i < noOfElements; i++) {
        ShaderType type;
        // Arbitrary assignment of shader types
        if (i == 0) {
            type = VERTEX_SHADER;
        }
        if (i == 1) {
            type = FRAGMENT_SHADER;
        }
        shader->contents[i] = "";
        shader->contents[i] = GLGetShaderContent(shader, type);
        if (!GLCompileShader(shader, type, i)) {
            return FALSE;
        }
        GLAttachShaderToProgram(shaderProgram, shader, type);
        if (StringContains(shader->contents[i], "uniform")) {
            shader->hasUniform = TRUE;
        }
    }
    return TRUE;
}

void GLDeleteShaders(Shader* shader) {
    for (i32 i = 0; i < 2; i++) {
        if (i == 0) {
            glDeleteShader(shader->vertexShader);
        }
        if (i == 1) {
            glDeleteShader(shader->fragmentShader);
        }
    }
}

b8 GLInitializeShaderProgram(u32* shaderProgram) {
    *shaderProgram = glCreateProgram();
    return TRUE;
}


b8 GLLinkShaderProgram(u32* shaderProgram) {
    glLinkProgram(*shaderProgram);

    if (!GLShaderCheckCompileProblems(shaderProgram, SHADER_PROGRAM)) {
        // No need for extra error flags
        return FALSE;
    }
    return TRUE;
}

b8 GLShaderCheckCompileProblems(u32* loc, ShaderType type) {
    string shaderTypes[2] = {"FRAGMENT_SHADER", "VERTEX_SHADER"};
    int success;
    char infoLog[MAX_LOG_ENTRY];
    // Check shader program linking
    if (type == SHADER_PROGRAM) {
        glGetProgramiv(*loc, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(*loc, MAX_LOG_ENTRY, NULL, infoLog);
            VERROR("Shader program linking error: \n%s", infoLog);
            return FALSE;
        }
    // Check shader compilation
    } else if (type == FRAGMENT_SHADER || type == VERTEX_SHADER) {
        glGetShaderiv(*loc, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(*loc, MAX_LOG_ENTRY, NULL, infoLog);
            VERROR("Shader compile error: \nTYPE: %s,\nLOG: %s\n", shaderTypes[type], infoLog);
            return FALSE;
        }
    // Type is invalid as there are no other ShaderTypes supported
    } else {
        VERROR("ShaderType is of invalid type. Please enter a valid ShaderType.");
        return FALSE;
    }
    // If all checks are passed
    return TRUE;
}
