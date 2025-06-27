#pragma once

#include "core/typedefs.h"

#include <atomic>

template <typename T>
class AtomicCounter {

	std::atomic<T> counter;
public:

    FORCE_INLINE T get() const {
        return counter.load(std::memory_order_acquire);
    }

    FORCE_INLINE void set(T p_value) {
        counter.store(p_value, std::memory_order_release);
    }

    FORCE_INLINE T add(T p_value) {
        return counter.fetch_add(p_value);
    }

    FORCE_INLINE T sub(T p_value) {
        return counter.fetch_sub(p_value);
    }

    FORCE_INLINE T increment() {
        return counter.fetch_add(1, std::memory_order_acq_rel) + 1;
    }

    FORCE_INLINE T decrement() {
        return counter.fetch_sub(1, std::memory_order_acq_rel) - 1;
    }

    FORCE_INLINE T conditional_increment() {
        while (true) {
            T c = counter.load(std::memory_order_acquire);
            if (c == 0) {
                return 0;
            }

            if (counter.compare_exchange_weak(c, c + 1, std::memory_order_acq_rel)) {
                return c + 1;
            }
        }
    }

    FORCE_INLINE void exchange_if_greater(T p_value) {
        T current = get();
        if (current < p_value) {
            set(p_value);
        }
    }

    FORCE_INLINE void exchange_if_lesser(T p_value) {
        T current = get();
        if (current > p_value) {
            set(p_value);
        }
    }

    AtomicCounter(T p_init = static_cast<T>(0)) {
        set(p_init);
    }
};

class Refcount {

	AtomicCounter<uint64> refc;
public:

	FORCE_INLINE bool ref() {
        return refc.conditional_increment() != 0;
    }

    FORCE_INLINE uint64 refval() {
        return refc.conditional_increment();
    }

    FORCE_INLINE bool unref() {
        return refc.decrement() == 0;
    }

    FORCE_INLINE uint64 unrefval() {
        return refc.decrement();
    }

    FORCE_INLINE void set(uint64 p_value) {
        refc.set(p_value);
    }

    FORCE_INLINE uint64 get() const {
        return refc.get();
    }

    FORCE_INLINE void init(uint64 p_value = 1) {
        refc.set(p_value);
    }
};
