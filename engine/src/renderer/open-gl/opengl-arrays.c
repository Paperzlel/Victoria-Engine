#include "opengl-arrays.h"

void GLGenerateArrayObject(u32* VAO) {
    glGenVertexArrays(1, VAO);
}

void GLBindVertexArray(u32 VAO) {
    glBindVertexArray(VAO);
}