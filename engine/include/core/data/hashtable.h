#pragma once

#include "hashfuncs.h"
#include "key_value.h"

#include "core/error/error_macros.h"
#include "core/os/memory.h"
#include "core/typedefs.h"

#include <type_traits>

// To read:
// - Hash key
// - Check against hash in slot
// - If hash is not equal, then increment pointer
// - Repeat until the equal hash has been found
// - Return key value

/**
 * @brief Hashtable that implements Robin-Hood hashing.
 */
template <typename TKey, typename TValue, typename Hasher = HasherDefault>
class HashTable {
	KeyValue<TKey, TValue> *hashed_data = nullptr;
	uint32_t *hashes = nullptr;
	uint64_t element_count = 0;
	uint32_t _prime_idx = 0;

	FORCE_INLINE uint32_t _hash(const TKey &p_key) const {
		uint32_t ret = Hasher::hash(p_key);
		if (ret == 0) {
			ret = ret + 1;
		}

		return ret;
	}

	void _inc_mod(uint32_t &r_mod, uint32_t p_capacity) const {
		r_mod++;
		if (r_mod == p_capacity) {
			r_mod = 0;
		}
	}

	void _resize_and_remap(uint32_t p_old_size, uint32_t p_new_size) {
		uint32_t *old_hashes = hashes;
		KeyValue<TKey, TValue> *old_data = hashed_data;

		hashed_data = (KeyValue<TKey, TValue> *)Memory::vallocate_zeroed(p_new_size * sizeof(KeyValue<TKey, TValue>));
		hashes = (uint32_t *)Memory::vallocate_zeroed(p_new_size * sizeof(uint32_t));
		element_count = 0;

		// No need to resize
		if (p_old_size == 0) {
			return;
		}

		// All elements prior to the new size need to be remapped
		for (uint32_t i = 0; i < p_old_size; i++) {
			if (old_hashes[i] == 0) {
				continue;
			}

			_insert_element(old_hashes[i], old_data[i], PRIMES[_prime_idx]);
		}

		Memory::vfree(old_hashes);
		Memory::vfree(old_data);
	}

	TValue &_find(uint32_t p_hash) const {
		uint32_t size = PRIMES[_prime_idx];
		int probe = 0;
		int idx = p_hash % size;

		while (idx + probe < size) {
			if (hashes[idx] == p_hash) {
				return ((KeyValue<TKey, TValue> *)(hashed_data + idx + probe))->value;
			}

			_inc_mod(idx, size);
			probe++;
		}

		ERR_COND_FATAL(true);
	}

	FORCE_INLINE uint32_t _get_probe_distance(uint32_t p_hash, uint32_t p_idx) const {
		uint32_t size = PRIMES[_prime_idx];
		uint32_t initail_idx = p_hash % size;

		return p_idx >= initail_idx ? p_idx - initail_idx : initail_idx;
	}

	bool _find_hashed_index(const TKey &p_key, uint32_t p_hash, uint32_t &r_idx) const {
		if (!hashed_data || element_count == 0) {
			return false;
		}

		uint32_t size = PRIMES[_prime_idx];
		uint32_t idx = p_hash % size;
		uint32_t probe_len = 0;

		while (true) {
			if (hashes[idx] == 0) {
				return false;
			}

			if (probe_len > _get_probe_distance(p_hash, idx)) {
				return false;
			}

			if (hashes[idx] == p_hash && hashed_data[idx].key == p_key) {
				r_idx = idx;
				return true;
			}

			probe_len++;
			_inc_mod(idx, size);
		}
	}

	KeyValue<TKey, TValue> *
	_insert_element(uint32_t p_hash, const KeyValue<TKey, TValue> &p_value, uint32_t p_table_size) {
		// Open hashing --> closed hashing allows us to keep the initial hash, useful for comparison
		uint32_t idx = p_hash % p_table_size;
		int probe_len = 0;
		KeyValue<TKey, TValue> data = p_value;
		uint32_t hash = p_hash;

		while (true) {
			// Found empty hash
			if (hashes[idx] == 0) {
				hashes[idx] = hash;
				hashed_data[idx] = data;
				element_count++;
				return &hashed_data[idx];
			}

			// Occupied, check hash distance
			int probe_dist = _get_probe_distance(hashes[idx], idx);
			if (probe_dist < probe_len) {
				// Swap hashes + data
				SWAP(hashes[idx], hash);
				SWAP(hashed_data[idx], data);
				// Recompute probe length and index
				probe_len = probe_dist;
				idx = (hash % p_table_size) + probe_dist;
			}

			probe_len++;
			_inc_mod(idx, p_table_size);
		}
	}

	KeyValue<TKey, TValue> *_insert(uint32_t p_hash, const KeyValue<TKey, TValue> &p_value) {
		uint32_t size = PRIMES[_prime_idx];

		// Need to allocate table
		if (hashed_data == nullptr) {
			hashed_data = (KeyValue<TKey, TValue> *)Memory::vallocate_zeroed(size * sizeof(KeyValue<TKey, TValue>));
			hashes = (uint32_t *)Memory::vallocate_zeroed(size * sizeof(uint32_t));
		}

		// Check if a resize is needed (most of the table is now being occupied)
		if (element_count + 1 > 0.8 * size) {
			_resize_and_remap(size, PRIMES[++_prime_idx]);
		}

		// Insert new element
		return _insert_element(p_hash, p_value, size);
	}

public:
	FORCE_INLINE bool is_empty() const {
		return element_count == 0;
	}
	FORCE_INLINE int64_t get_element_count() const {
		return element_count;
	}

	FORCE_INLINE bool has(const TKey &p_key) const {
		uint32_t h = _hash(p_key);
		uint32_t tmp;
		return _find_hashed_index(p_key, h, tmp);
	}

	FORCE_INLINE TValue &get(const TKey &p_key) {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		ERR_COND_FATAL(!exists);
		return hashed_data[idx].value;
	}

	FORCE_INLINE const TValue &get(const TKey &p_key) const {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		ERR_COND_FATAL(!exists);
		return hashed_data[idx].value;
	}

	FORCE_INLINE TValue *get_ptr(const TKey &p_key) {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		if (exists) {
			return &hashed_data[idx].value;
		}

		return nullptr;
	}

	FORCE_INLINE const TValue *get_ptr(const TKey &p_key) const {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		if (exists) {
			return &hashed_data[idx].value;
		}

		return nullptr;
	}

	FORCE_INLINE TValue &operator[](const TKey &p_key) {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		if (!exists) {
			return _insert(h, KeyValue<TKey, TValue>(p_key, TValue()))->value;
		}

		return hashed_data[idx].value;
	}

	FORCE_INLINE const TValue &operator[](const TKey &p_key) const {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		ERR_COND_FATAL(!exists);
		return hashed_data[idx].value;
	}

	FORCE_INLINE void insert(const TKey &p_key, const TValue &p_value) {
		uint32_t h = _hash(p_key);
		_insert(h, KeyValue<TKey, TValue>(p_key, p_value));
	}

	FORCE_INLINE bool erase(const TKey &p_key) {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		ERR_FAIL_COND_MSG_R(!exists, "Key does not exist in table", false);

		hashes[idx] = 0;
		hashed_data[idx].key.~TKey();
		hashed_data[idx].value.~TValue();
		// Removing an item should mean that every item with the same hash is moved down a step
		// Check hash idx + 1
		// If hash is valid, move down. Invalidate hash idx + 1
		// Repeat at idx + 2
		uint32_t next_idx = idx + 1;
		uint32_t size = PRIMES[_prime_idx];
		while (next_idx < size) {
			if (hashes[next_idx] != 0) {
				// Get closed hash (should point to next index)
				uint32_t h2 = hashes[next_idx] % size;

				if (h2 == idx) {
					SWAP(hashed_data[next_idx - 1], hashed_data[next_idx]);
				} else {
					break;
				}
			}

			next_idx++;
		}

		element_count--;
		return true;
	}

	FORCE_INLINE HashTable(uint64_t p_first_prime = 1) {
		_prime_idx = p_first_prime;
	}
	FORCE_INLINE ~HashTable() {
		if (!std::is_trivially_destructible<TValue>::value) {
			for (int i = 0; i < get_element_count(); i++) {
				TValue *t = &hashed_data[i].value;
				t->~TValue();
			}
		}

		Memory::vfree(hashed_data);
	}
};
