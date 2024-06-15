#pragma once

#include "defines.h"

#include "opengl-types.inl"

/* File for the Vertex Array Object handling. This file is pretty small, so it's likely it gets merged with the buffer files at a later date. */

/* 
 * @brief Function to generate a vertex buffer object
 * @param VAO A pointer to a u32 that we will bind to an object
 */
void GLGenerateArrayObject(u32* VAO);

/* 
 * @brief Binds a vertex array object to be used 
 * @param VAO A u32 of a VAO that needs to be bound 
 */
void GLBindVertexArray(u32 VAO);

