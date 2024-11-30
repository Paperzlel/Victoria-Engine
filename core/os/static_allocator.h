#ifndef STATIC_ALLOCATOR_H
#define STATIC_ALLOCATOR_H

#include "core/typedefs.h"

class StaticAllocator {
public:

    static void *vallocate(uint64 size);
    static void *vreallocate(void *block, uint64 old_size, uint64 new_size);
    static void vfree(void *block, uint64 size);
    static void vzero(void *block, uint64 size);
    static void vset_memory(void *block, int value, uint64 size);
    static void *vcopy_memory(void *dest, void *source, uint64 size);
};

#endif