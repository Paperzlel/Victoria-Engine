#include "opengl-shaders.h"

#include "core/logger.h"
#include "core/vmemory.h"
#include "core/vstring.h"

#include "systems/file-system.h"

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

    FileHandler fh;
    fh.handle = 0;
    if (!OpenFile(&fh, totalFilePath, ACCESS_TYPE_READ)) {
        VERROR("Failed to load the file.");
        return "";
    }

    u64 fileSize = GetFileSize(&fh);

    char* shaderContent = VAllocate(sizeof(char) * fileSize, MEMORY_TAG_ARRAY); // Memory is allocated but kept in the contents of the shader so no need to unload
    shaderContent = VZero(shaderContent, sizeof(char) * fileSize);
    GetFileContentsAsText(&fh, totalFilePath, shaderContent);
    // HACK: Setting EOF manually to prevent issues??
    shaderContent[fileSize] = '\0';
    return shaderContent;
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

b8 GLListAllUniforms(Shader* shader, u8 index) {
    //u8 uniformArraySize = 16;
    u32 pos = 0;
    //b8 hasRegisteredType = FALSE;

    char* contents = (char*)shader->contents[index];

    // TODO: Give a max line length
    char buf[300];

    if (StringContains(shader->contents[index], "uniform", &pos)) {
        shader->hasUniform = TRUE;
        // offset to the first character of the uniform type
        pos += 3;
        u16 linePos = 0;
        while (contents[pos] != ('\n' | '\0' | ';')) {
            buf[linePos] = contents[pos];
            linePos++;
            pos++;
        }
        buf[linePos + 1] = '\0';

        StringArray arr = CreateStringArray(2);

        StringSplit(buf, " ", &arr);

        //TODO: List more than one uniform
        shader->uniformTypes.contents[0] = arr.contents[0];
        shader->uniforms.contents[0] = arr.contents[1];
        return TRUE;
    }

    return FALSE;
}

//NOTE: At the moment allocation of uniforms is not fully implemented, the one thing to do before this is to  allow for custom code in some way.
//      This means that for now I will not be able to use uniforms in this way and to use your own uniforms one will have to implement them
//      manually.
b8 GLUseUniform(string uniformType, string uniformName) {
    return TRUE;
}

b8 GLRegisterNewShader(Shader* shader, u32* shaderProgram) {
    //TODO: Add functions to find length of an array
    i32 noOfElements = 2;

    shader->uniforms = CreateStringArray(16);
    shader->uniformTypes = CreateStringArray(16);

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
        if (!GLListAllUniforms(shader, i)) {
            VERROR("Unable to list all the uniforms. Assuming none.");
            shader->hasUniform = FALSE;
        }
        // Attempt to reload with a different shader instead of booting out
        if (!GLCompileShader(shader, type, i)) {
            VERROR("Could not compile the shader. See above for the compile error");
        }
        GLAttachShaderToProgram(shaderProgram, shader, type);
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
        VFree((char*)shader->contents[i], StringLength(shader->contents[i]), MEMORY_TAG_ARRAY);
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
            VERROR("Shader compile error: \nTYPE: %s,\nLOG: %s", shaderTypes[type], infoLog);
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
