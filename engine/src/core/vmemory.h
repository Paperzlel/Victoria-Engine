#pragma once 

#include "defines.h"

#include "core/logger.h"

/* File to handle memory allocation.
 * VOID POINTER TIME WOOO!
 * In essence, a void pointer (void*) is data that has no associated data type
 * This means that we can typecast it to anything we want to, if needs be. 
 */

typedef enum MemoryTagTypes {
    MEMORY_TAG_APPLICATION,
    MEMORY_TAG_ARRAY,
    MEMORY_TAG_FILE,
    MEMORY_TAG_SCENE,
    MEMORY_TAG_NODE, // Or something
    MEMORY_TAG_NONE,
    MEMORY_TAG_RENDERER,
    MEMORY_TAG_MATERIAL,
    MEMORY_TAG_MISC,
    //TODO: Add more tags
    MEMORY_TAG_MAX
} MemoryTagTypes;

/* 
 * @brief The method to being memory allocation.
 * @return TRUE if successful, FALSE if not.
 */
b8 MemoryInitialize();

/* 
 * @brief A method that allocates a specific type of memory of a given size
 * @param size The size of the memory block in bytes
 * @param type A MemoryTagType of the chosen memory type
 * @return A void pointer for the allocated memory
 */
void* VAllocate(u64 size, MemoryTagTypes type);

/* 
 * @brief Method to re-allocate memory to a new size
 * @param *memory A pointer to the memory to resize
 * @param oldSize The previously allocated memory size
 * @param newSize The new memory size
 * @param type The type of memory we are allocating
 * @return A pointer to the new memory size
 */
void* VReallocate(void* memory, u64 oldSize, u64 newSize, MemoryTagTypes type);

/*
 * @brief A method to set all of the data in a given block to zero
 * @param block The block of memory to zero out
 * @param size The size of the given memory block in bytes
 * @param type The type of memory that is being zeroed out (tracking purposes)
 * @return A void pointer for the now zeroed-out memory
 */
void* VZero(void* block, u64 size);

/*
 * @brief A method to free previously allocated memory from the engine
 * @param block The block of memory to free
 * @param size The size of the memory block in bytes
 * @param types The type of memory that is being freed (tracking purposes)
 */
void VFree(void* block, u64 size, MemoryTagTypes type);

/* 
 * @brief A method to set a given part of memory to a specific value
 * @param loc The location of the memory to set
 * @param value The value we want to set the memory to
 * @param size The size of the memory in bytes
 * @return A void pointer to the now-set memory
 */
void* VSetMemory(void* loc, i32 value, u64 size);

/*
 * @brief The method that logs the current application memory usage in bytes
 */
VAPI void LogMemoryUsage();