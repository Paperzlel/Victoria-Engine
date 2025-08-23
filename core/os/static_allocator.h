#pragma once

#include "core/typedefs.h"

class StaticAllocator {
public:

    static void *vallocate(u64 size);
    static void *vreallocate(void *block, u64 old_size, u64 new_size);
    static void vfree(void *block, u64 size);
    static void vzero(void *block, u64 size);
    static void vset_memory(void *block, int value, u64 size);
    static void *vcopy_memory(void *dest, void *source, u64 size);
};
