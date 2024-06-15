#include "opengl-buffers.h"

#include "core/logger.h"

b8 RegisterNewBufferObject(u32* obj, BufferType type) {
    // If the object is already registered
    if (glIsBuffer(*obj)) {
        VWARN("Buffer object given is already a registered buffer object.");
        return FALSE;
    }
    // Create and bind the buffer
    // the 1 is the number of buffers to create, obj is the memory address of the object
    glGenBuffers(1, obj);
    glBindBuffer(type, *obj);
    return TRUE;
}
