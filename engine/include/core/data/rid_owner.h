#pragma once

#include "atomic_counter.h"
#include "rid.h"
#include "vector.h"

#include "core/os/memory.h"
#include "core/typedefs.h"

class RIDAllocatorBase {
	static AtomicCounter<uint64_t> global_counter;

public:
	static RID _gen_rid() {
		RID rid;
		rid._id = _gen_id();
		return rid;
	}

	static uint64_t _gen_id() {
		return global_counter.increment();
	}

	static RID _gen_from_uint(uint64_t p_uint) {
		RID rid;
		rid._id = p_uint;
		return rid;
	}
};

template <typename T>
class RIDAllocator : public RIDAllocatorBase {
	// Items are the base of the RID allocations. They contain the value to hold, as well as a validator to let us know
	// if the position is free or not.
	struct Item {
		T data;
		uint32_t validator;
	};

	// Pointer of pointers for chunks and their arrays
	Item **items = nullptr;
	// Free list of the next available RIDs
	uint32_t **free_list = nullptr;

	// Maximum number of chunks that the allocator can hold
	uint64_t max_chunks;
	// The maximumn number of allocations that can occur before a new chunk is needed
	uint64_t max_allocations = 0;
	// The number of items currently allocated
	uint64_t items_allocated = 0;
	// The maximum number of items a chunk can hold
	uint64_t items_per_chunk;

	FORCE_INLINE RID _allocate_rid() {
		if (items_allocated == max_allocations) {
			uint64_t new_chunk_count = max_allocations / items_per_chunk;

			ERR_FAIL_COND_MSG_R(new_chunk_count == max_chunks, "Item RID limit reached.", RID());

			items = (Item **)Memory::vreallocate(items, sizeof(Item *) * (new_chunk_count + 1));
			items[new_chunk_count] = (Item *)Memory::vallocate(sizeof(Item) * items_per_chunk);

			free_list = (uint32_t **)Memory::vreallocate(free_list, sizeof(uint32_t *) * (new_chunk_count + 1));
			free_list[new_chunk_count] = (uint32_t *)Memory::vallocate(sizeof(uint32_t) * items_per_chunk);

			for (uint64_t i = 0; i < items_per_chunk; i++) {
				items[new_chunk_count][i].validator = 0xffffffff; // Set to invalid ID
				free_list[new_chunk_count][i] = items_allocated + i;
			}
			max_allocations += items_per_chunk;
		}

		// The next available RID slot in the current chunk
		uint64_t first_free = free_list[items_allocated / items_per_chunk][items_allocated % items_per_chunk];

		uint64_t chunk_id = first_free / items_per_chunk;
		uint64_t item_idx = first_free % items_per_chunk;

		uint32_t validator = uint32_t(_gen_id() & 0x7fffffff); // Check for RID overflow
		ERR_COND_FATAL(validator == 0x7fffffff);
		uint64_t id = validator;
		id <<= 32;
		id |= first_free;

		items[chunk_id][item_idx].validator = validator; // Mark last bit as 1 so it can be read as un-initialized
		items[chunk_id][item_idx].validator |= 0x80000000;

		items_allocated++;

		return _gen_from_uint(id);
	}

public:
	RID make_rid() {
		RID rid = _allocate_rid();
		initialize_rid(rid);
		return rid;
	}

	RID make_rid(T &p_value) {
		RID rid = _allocate_rid();
		initialize_rid(rid, p_value);
		return rid;
	}

	RID allocate_rid() {
		return _allocate_rid();
	}

	FORCE_INLINE T *get_or_null(const RID &p_rid, bool p_initialize = false) {
		if (p_rid == RID()) {
			return nullptr;
		}

		uint64_t id = p_rid.get_id();
		uint32_t index = uint32_t(id & 0xffffffff);

		uint64_t chunk_id = index / items_per_chunk;
		uint64_t chunk_idx = index % items_per_chunk;

		uint32_t validator = uint32_t(id >> 32);

		Item &i = items[chunk_id][chunk_idx];

		// Initialize the RID as in use
		if (p_initialize) {
			ERR_FAIL_COND_MSG_R(!(i.validator & 0x80000000), "RID was already initialized.", nullptr);
			ERR_FAIL_COND_MSG_R((i.validator & 0x7fffffff) != validator,
								"Attempted to initialize the wrong RID.",
								nullptr);

			items[chunk_id][chunk_idx].validator &= 0x7fffffff;
		} else if (i.validator != validator) {
			ERR_FAIL_COND_MSG_R(i.validator & 0x80000000, "Attempted to use an uninitialized RID.", nullptr);
			return nullptr;
		}

		T *ptr = &i.data;
		return ptr;
	}

	void initialize_rid(const RID &p_rid) {
		T *ptr = get_or_null(p_rid, true);
		ERR_COND_NULL(ptr);
		vnew_placement(ptr, T);
	}

	void initialize_rid(const RID &p_rid, T &p_value) {
		T *ptr = get_or_null(p_rid, true);
		ERR_COND_NULL(ptr);
		vnew_placement(ptr, T(p_value));
	}

	FORCE_INLINE bool owns(const RID &p_rid) const {
		uint64_t id = p_rid.get_id();
		uint32_t index = uint32_t(id & 0xffffffff);

		uint64_t chunk_id = index / items_per_chunk;
		uint64_t chunk_idx = index % items_per_chunk;

		uint32_t validator = id >> 32;
		bool owned = (validator != 0x7fffffff) && (items[chunk_id][chunk_idx].validator & 0x7fffffff) == validator;
		return owned;
	}

	FORCE_INLINE void free(const RID &p_rid) {
		uint64_t id = p_rid.get_id();
		uint64_t index = uint32_t(id & 0xffffffff);

		uint64_t chunk_id = index / items_per_chunk;
		uint64_t chunk_idx = index % items_per_chunk;

		uint32_t validator = uint32_t(id >> 32);
		ERR_FAIL_COND_MSG(items[chunk_id][chunk_idx].validator & 0x80000000,
						  "Attempted to free an uninitialized RID.");
		ERR_FAIL_COND(items[chunk_id][chunk_idx].validator != validator);

		items[chunk_id][chunk_idx].data.~T();
		items[chunk_id][chunk_idx].validator = 0xffffffff;

		items_allocated--;
		free_list[items_allocated / items_per_chunk][items_allocated % items_per_chunk] =
			index; // Make the now-freed item the next spot on the free list
	}

	FORCE_INLINE void get_owned_list(Vector<RID> *p_list) const {
		for (uint64_t i = 0; i < max_allocations; i++) {
			uint64_t validator = items[i / items_per_chunk][i % items_per_chunk].validator;
			if (validator != 0xffffffff) {
				p_list->push_back(_gen_from_uint((validator << 32) | i));
			}
		}
	}

	RIDAllocator(uint64_t max_chunk_size = 16384, uint64_t max_item_count = 65536) {
		max_chunks = max_item_count / max_chunk_size; // By default, we have 4 chunks.
		items_per_chunk = sizeof(T) > max_chunk_size ? 1 : (max_chunk_size / sizeof(T));
	}

	~RIDAllocator() {
		if (items_allocated) {
			for (uint64_t i = 0; i < max_allocations; i++) {
				uint64_t validator = items[i / items_per_chunk][i % items_per_chunk].validator;
				if (validator & 0x80000000) {
					continue; // Never initialized
				}
				// Was initialized and has not yet been freed
				if (validator != 0xffffffff) {
					items[i / items_per_chunk][i % items_per_chunk].data.~T();
				}
			}
		}

		uint32_t chunks_used = max_allocations / items_per_chunk;
		for (uint32_t i = 0; i < chunks_used; i++) {
			Memory::vfree(items[i]);
			Memory::vfree(free_list[i]);
		}

		if (items) {
			Memory::vfree(items);
			Memory::vfree(free_list);
		}
	}
};

/**
 * An RID Owner stores the data associated with the RID as well as a validator code that associates to the bitwise
 * value of the RID. Every RID stores its bit value in the 1st half of the number (reading lowest to highest from right
 * to left) An RID "item" is a struct of the actual item itself and its validation code (a 32-bit uint which is equal
 * to the RID shifted 32 bits to the right) Since these allocators need to be fast if dealing with many different RIDs,
 * we want to be more spare with memory allocations. "Chunks" are regions of memory that can allow for multiple
 * different RID positions to be set up.
 *
 */

template <typename T>
class RIDOwner {
	RIDAllocator<T> allocator;

public:
	FORCE_INLINE RID make_rid() {
		return allocator.make_rid();
	}

	FORCE_INLINE RID make_rid(T &p_value) {
		return allocator.make_rid(p_value);
	}

	FORCE_INLINE RID allocate_rid() {
		return allocator.allocate_rid();
	}

	FORCE_INLINE T *get_or_null(const RID &p_rid, bool p_initialize = false) {
		return allocator.get_or_null(p_rid, p_initialize);
	}

	FORCE_INLINE void initialize_rid(const RID &p_rid) {
		allocator.initialize_rid(p_rid);
	}

	FORCE_INLINE void initialize_rid(const RID &p_rid, T &p_value) {
		allocator.initialize_rid(p_rid, p_value);
	}

	FORCE_INLINE bool owns(const RID &p_rid) const {
		return allocator.owns(p_rid);
	}

	FORCE_INLINE void free(const RID &p_rid) {
		allocator.free(p_rid);
	}

	FORCE_INLINE void get_owned_list(Vector<RID> *p_list) const {
		allocator.get_owned_list(p_list);
	}

	RIDOwner(uint64_t max_chunk_size = 16384, uint64_t max_item_count = 65536) :
		allocator(max_chunk_size, max_item_count) {}
};
