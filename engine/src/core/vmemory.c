#include "vmemory.h"

#include <stdlib.h>
#include <string.h>

struct MemoryStats {
    u64 totalMemoryUsage;
};

static struct MemoryStats stats;

u64 previousMemoryUsage = 0;

void* VAllocate(u64 size, MemoryTagTypes type) {
    if (type == MEMORY_TAG_NONE) {
        VWARN("Memory has undefined tag type. Please redefine its memory type.");
    }
    stats.totalMemoryUsage += size;
    return malloc(size);
}

void* VReallocate(void* memory, u64 oldSize, u64 newSize, MemoryTagTypes type) {
    if (type == MEMORY_TAG_NONE) {
        VWARN("Memory has unknown tag type. Please redefine its memory type");
    }

    if (oldSize > newSize) {
        stats.totalMemoryUsage -= (oldSize - newSize);
    } else {
        stats.totalMemoryUsage += (newSize - oldSize);
    }
    return realloc(memory, newSize);
}

void* VZero(void* block, u64 size) {
    return memset(block, 0, size);
}

void* VSetMemory(void* dest, i32 value, u64 size) {
    return memset(dest, value, size);
}

void VFree(void* block, u64 size, MemoryTagTypes type) {
    stats.totalMemoryUsage -= size;
    free(block);
}

// TODO: Per-tag allocations
void LogMemoryUsage() {
    // Checks to see if memory usage has changed 
    if (stats.totalMemoryUsage != previousMemoryUsage) {
        VDEBUG("Current memory usage (bytes): %u", stats.totalMemoryUsage);
        previousMemoryUsage = stats.totalMemoryUsage;
    }
}