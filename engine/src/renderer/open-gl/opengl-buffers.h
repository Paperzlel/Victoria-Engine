#pragma once

#include "defines.h"

#include "opengl-types.inl"

/* Header file for buffer objects.
 * There's 3 ways of setting up object data:
 * 1. (VVV) (NNN) (CCC) - storing vertex, normal and colour data in separate buffers
 * 2. (VVVNNNCCC) - Storing data in one buffer in a series of arrays
 * 3. (VNCVNCVNC) - Storing data per-vertex adjacent to one another (need glVertexAttribPointer)
 */

// The different types of buffer we can have (read https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml for more info)
typedef enum BufferType {
    ARRAY_BUF = GL_ARRAY_BUFFER,
    ATOMIC_COUNTER_BUF = GL_ATOMIC_COUNTER_BUFFER,
    COPY_READ_BUF = GL_COPY_READ_BUFFER,
    COPY_WRITE_BUF = GL_COPY_WRITE_BUFFER,
    DISPATCH_INDIRECT_BUF = GL_DISPATCH_INDIRECT_BUFFER,
    DRAW_INDIRECT_BUF = GL_DRAW_INDIRECT_BUFFER,
    ELEMENT_ARRAY_BUF = GL_ELEMENT_ARRAY_BUFFER,
    PIXEL_PACK_BUF = GL_PIXEL_PACK_BUFFER,
    PIXEL_UNPACK_BUF = GL_PIXEL_UNPACK_BUFFER,
    QUERY_BUF = GL_QUERY_BUFFER,
    SHADER_STORAGE_BUF = GL_SHADER_STORAGE_BUFFER,
    TEXTURE_BUF = GL_TEXTURE_BUFFER,
    TRANSFORM_FEEDBACK_BUF = GL_TRANSFORM_FEEDBACK_BUFFER,
    UNIFORM_BUF = GL_UNIFORM_BUFFER
} BufferType;


/* @brief Registering a new buffer object to OpenGL
 * @param obj A reference to the object we are registering 
 * @param type The type of buffer object we are using
 * @return Returns TRUE if the object registered successfully
 */
b8 RegisterNewBufferObject(u32* obj, BufferType type);

void DeleteBufferObject(u32* obj);