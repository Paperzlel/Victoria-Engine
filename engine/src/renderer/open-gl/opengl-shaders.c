#include "opengl-shaders.h"

#include "core/logger.h"

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

char* GLGetShaderContent(ShaderObject* object) {
    FILE* fptr;
    f64 size = 0;
    char* shaderFileContent;

    /* Read file to get size*/
    fptr = fopen(object->filepath, "rb");
    if (fptr == NULL) {
        return "";
    }
    fseek(fptr, 0L, SEEK_END);
    size = ftell(fptr)+1;
    fclose(fptr);

    //TODO: Custom memory allocation
    fptr = fopen(object->filepath, "r");
    shaderFileContent = memset(malloc(size), '\0', size);
    fread(shaderFileContent, 1, size-1, fptr);
    fclose(fptr);

    //TODO: Return shader file content to struct
    return shaderFileContent;
}

b8 GLCompileShader(ShaderObject* object) {
    u32 shader;
    if (!object) {
        VERROR("Shader is null!");
        return FALSE;
    }

    if (object->type == VERTEX_SHADER) {
        shader = glCreateShader(GL_VERTEX_SHADER);
    } else if (object->type == FRAGMENT_SHADER) {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    } else {
        VERROR("Unsupported shader type!");
        return FALSE;
    }

    glShaderSource(shader, 1, &object->content, NULL);
    glCompileShader(shader);

    //TODO: Maybe add a max log size to defines.h?
    int success;
    char infoLog[32000];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 32000, NULL, infoLog);
        VERROR("Shader compilation failed: \n%s\n", infoLog);
        return FALSE;
    }

    object->GLShader = shader;
    return TRUE;
}

b8 GLShaderInitializeShaderProgram(u32* shaderProgram) {
    *shaderProgram = glCreateProgram();
    //TODO: Temporary code. Checks to see if the dereferencing is working. Also find a way to check for bugs
    return TRUE;
}

b8 RegisterNewShaderObject(ShaderObject* object, u32* shaderProgram) {
    object->content = GLGetShaderContent(object);
    //TODO: String comparison file
    //if (object.content == "") {
    //    VERROR("Shader has no file contents!");
    //    return FALSE;
    //}

    if (!GLCompileShader(object)) {
        VERROR("Shader could not be compiled!");
        return FALSE;
    }

    // Attach shader to shader program
    glAttachShader(*shaderProgram, object->GLShader);
    //Delete shader once it is no longer needed to save memory
    glDeleteShader(object->GLShader);
    return TRUE;
}

b8 GLLinkShaderProgram(u32* shaderProgram) {
    glLinkProgram(*shaderProgram);

    int success;
    char infoLog[32000];
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(*shaderProgram, 32000, NULL, infoLog);
        VERROR("Shader program failed to link! \n%s\n", infoLog);
        return FALSE;
    }
    return TRUE;
}