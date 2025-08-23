#include "static_allocator.h"

#include <stdlib.h>
#include <string.h>

void *StaticAllocator::vallocate(u64 size) {
    return malloc(size);
}

void *StaticAllocator::vreallocate(void *block, u64 old_size, u64 new_size) {
    return realloc(block, new_size);
}

void StaticAllocator::vfree(void *block, u64 size) {
    if (!block) {
        return;
    }

    free(block);
}

void StaticAllocator::vzero(void *block, u64 size) {
    memset(block, 0, size);
}

void StaticAllocator::vset_memory(void *dest, int value, u64 size) {
    memset(dest, value, size);
}

void *StaticAllocator::vcopy_memory(void *dest, void *source, u64 size) {
    return memcpy(dest, source, size);
}