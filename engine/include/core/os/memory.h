#pragma once

#include "core/typedefs.h"
#include "core/data/atomic_counter.h"

class VAPI Memory {
public:

    static AtomicCounter<u64> current_mem_usage;
    static AtomicCounter<u64> max_mem_usage;

    static constexpr size_t DATA_OFFSET = sizeof(size_t);

    static u64 get_memory_usage();
    static u64 get_mem_max_usage();

    static void *vallocate(u64 p_size);
    static void *vallocate_zeroed(u64 p_size);
    static void *vreallocate(void *p_block, u64 p_new_size);
    static void vfree(void *p_block);
    static void vzero(void *p_block, u64 p_size);
    static void vset_memory(void *p_block, int p_value, u64 p_size);
    static void *vcopy_memory(void *p_dest, void *p_source, u64 p_size);

};

VAPI void *operator new(size_t p_size, const char *p_description);

#define vnew(m_class) ::new ("") m_class
#define vnew_placement(m_placement, m_class) ::new (m_placement) m_class

ALWAYS_INLINE bool predelete(void *) {
    return true;
}

template<typename T>
void vdelete(T *p_class) {
    if (!p_class) {
        return;
    }

    if (!predelete(p_class)) {
        return;
    }

    if (!std::is_trivially_destructible<T>::value) {
        p_class->~T();
    }

    Memory::vfree(p_class);
}