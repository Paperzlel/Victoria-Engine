#pragma once

#include "core/typedefs.h"

#include <atomic>

/**
 * @brief Template class that increments a thread-safe counter when called. T must be a valid, countable type.
 */
template <typename T>
class AtomicCounter {

    // The counter object itself.
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

    /**
     * @brief Increments the counter if the value hasn't been modified on a separate thread.
     * @returns The incremented value.
     */
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

    /**
     * @brief Swaps the value held by the counter if the given value is larger.
     * @param p_value The value to compare with
     */
    FORCE_INLINE void exchange_if_greater(T p_value) {
        T current = get();
        if (current < p_value) {
            set(p_value);
        }
    }

    /**
     * @brief Swaps the value held by the counter if the given value is lesser.
     * @param p_value The value to compare with
     */
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

/**
 * @brief Implementation of AtomicCounter for reference counting.
 */
class Refcount {

	AtomicCounter<u64> refc;
public:

    /**
     * @brief Increment the counter.
     * @returns True if successful, false if not.
     */
	FORCE_INLINE bool ref() {
        return refc.conditional_increment() != 0;
    }

    /**
     * @brief Increments the counter.
     * @returns The value held by the counter.
     */
    FORCE_INLINE u64 refval() {
        return refc.conditional_increment();
    }

    /**
     * @brief Dereferences the counter.
     * @returns True if successful, false if not.
     */
    FORCE_INLINE bool unref() {
        return refc.decrement() == 0;
    }

    /**
     * @brief Dereferences the counter.
     * @returns The value held by the counter.
     */
    FORCE_INLINE u64 unrefval() {
        return refc.decrement();
    }

    /**
     * @brief Assigns the refcount to a given value.
     * @param p_value The value to assign
     */
    FORCE_INLINE void set(u64 p_value) {
        refc.set(p_value);
    }

    /**
     * @brief Obtains the refcount.
     * @returns The refcount.
     */
    FORCE_INLINE u64 get() const {
        return refc.get();
    }

    /**
     * @brief Initializes the ref-counter to a given value. Should be called prior to use, as refcounts of 0 imply destruction at first.
     * @param p_value The value to set the counter to start at. Default is 1.
     */
    FORCE_INLINE void init(u64 p_value = 1) {
        refc.set(p_value);
    }
};
