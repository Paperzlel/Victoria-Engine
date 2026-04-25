#pragma once

#include "hashfuncs.h"
#include "key_value.h"

#include "core/error/error_macros.h"
#include "core/os/memory.h"
#include "core/typedefs.h"

template <typename TKey, typename TValue>
struct HashTableElement {
	HashTableElement<TKey, TValue> *next = nullptr;
	HashTableElement<TKey, TValue> *prev = nullptr;
	KeyValue<TKey, TValue> data;
	HashTableElement() {}
	HashTableElement(const TKey &p_key, const TValue &p_value) :
		data(p_key, p_value) {}
};

/**
 * @brief Hashtable that implements Robin-Hood hashing. Pointers and references held to data in the hashtable remain
 * under permutations, making it a very secure datatype at the cost of being expensive to allocate information for.
 * `TKey` is the datatype to hash against, and must have a valid `Hasher` function (or its own custom hasher) to work.
 * The `Allocator` needs to be typed and implement the methods `new_allocation` and `delete_allocation`. Most data
 * won't need this to function normally.
 */
template <typename TKey,
		  typename TValue,
		  typename Hasher = HasherDefault,
		  typename Allocator = DefaultTypedAllocator<HashTableElement<TKey, TValue>>>
class HashTable : private Allocator {
	// Array of pointers to `HashTableElements`. The data itself is stored in the pointers, not this array, hence the
	// permutability.
	HashTableElement<TKey, TValue> **hashed_data = nullptr;
	// Array of hashes. Their position correlates to their own value % the current size of the table.
	uint32_t *hashes = nullptr;
	// A pointer to the first element in the `HashTable`.
	HashTableElement<TKey, TValue> *_head = nullptr;
	// A pointer to the last element in the `HashTable`.
	HashTableElement<TKey, TValue> *_tail = nullptr;

	// The number of elements in the `HashTable`.
	uint64_t element_count = 0;
	// The index into the `PRIMES` array that the current capacity exists at.
	uint32_t _prime_idx = 0;

	/**
	 * @brief Hashes the given key into an unsigned 32-bit integer. If the hash function returns `0`, this function
	 * will always return 1. The hash function used can be modified by setting `typename Hasher` in the class
	 * construction.
	 * @param p_key The key to hash into an unsigned 32-bit integer. Must have a valid overloaded hashing function.
	 * @return An unsigned 32-bit hash value.
	 */
	FORCE_INLINE uint32_t _hash(const TKey &p_key) const {
		uint32_t ret = Hasher::hash(p_key);
		if (ret == 0) {
			ret = ret + 1;
		}

		return ret;
	}

	/**
	 * @brief Increments a counter and wraps the value around if it reaches a given capacity.
	 * @param r_mod The value to increment or reset.
	 * @param p_capacity The capacity of the given `HashTable`.
	 */
	void _inc_mod(uint32_t &r_mod, uint32_t p_capacity) const {
		r_mod++;
		if (r_mod == p_capacity) {
			r_mod = 0;
		}
	}

	/**
	 * @brief Resizes the current `HashTable` to fit a new number of elements. The sizes refer to the previous and next
	 * capacities, which should be prime numbers. Keeps any references to currently-held data around so that pointers
	 * can retain valid data under permutations.
	 * @param p_old_size The old capacity of the array.
	 * @param p_new_size The new capacity of the array.
	 */
	void _resize_and_remap(uint32_t p_old_size, uint32_t p_new_size) {
		HashTableElement<TKey, TValue> **old_data = hashed_data;
		uint32_t *old_hashes = hashes;

		hashed_data = (HashTableElement<TKey, TValue> **)Memory::vallocate_zeroed(
			p_new_size * sizeof(HashTableElement<TKey, TValue> *));
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

	/**
	 * @brief Finds a given value based on its hash alone. If the value could not be found, it crashes the engine. Do
	 * NOT use this function in code unless you are confident that the hash corresponds to a valid key-value pair.
	 * @param p_hash The precalculated hash to find the value for.
	 * @return A reference to the value corresponding to the given hash.
	 */
	TValue &_find(uint32_t p_hash) const {
		uint32_t size = PRIMES[_prime_idx];
		int probe = 0;
		uint32_t idx = p_hash % size;

		while (idx + probe < size) {
			if (hashes[idx] == p_hash) {
				return ((KeyValue<TKey, TValue> *)(hashed_data + idx + probe))->value;
			}

			_inc_mod(idx, size);
			probe++;
		}

		CRASH_NOW();
	}

	/**
	 * @brief Obtains the "probe distance", or the distance from the current index to the next valid hash location.
	 * @param p_hash The precalculated hash to find the distance to.
	 * @param p_idx The index into the `HashTable`'s data to locate it at.
	 * @return The probing distance of the hash and the respective index.
	 */
	FORCE_INLINE uint32_t _get_probe_distance(uint32_t p_hash, uint32_t p_idx) const {
		uint32_t size = PRIMES[_prime_idx];
		uint32_t initail_idx = p_hash % size;

		return p_idx >= initail_idx ? p_idx - initail_idx : initail_idx;
	}

	/**
	 * @brief Finds the index into the `HashTable` where a given key and its hash exist. Returns the index in `r_idx`.
	 * @param p_key The key to check for the existence of. There should be a check for hash collisions (where two equal
	 * hashes exist on non-equal keys) but that check will be added later.
	 * @param p_hash The precalculated hash to find the index of. Saves having to re-hash the key in this function,
	 * which could wind up being a lengthy process.
	 * @param r_idx The return index to place the value into. If the function returns `false`, then the contents of
	 * this value are undefined and should be ignored.
	 * @return `true` if the key was valid and has a defined location in the `HashTable`, `false` if it does not exist
	 * or that it hasn't been inserted into the `HashTable`.
	 */
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

			if (hashes[idx] == p_hash && hashed_data[idx]->data.key == p_key) {
				r_idx = idx;
				return true;
			}

			probe_len++;
			_inc_mod(idx, size);
		}
	}

	/**
	 * @brief Internal internal insertion method. Takes the precalculated hash, a pointer to the new `HashTableElement`
	 * object that was created, and the current size of the table.
	 * @param p_hash The precalculated hash. See `_insert` for more information.
	 * @param p_value A pointer to the `HashTableElement` object that contains the new data. See `_insert` for more
	 * information.
	 * @param p_table_size The current size of the table. Passed as we use `_prime_idx` to get our capacity which is
	 * expensive to call multiple times.
	 */
	void _insert_element(uint32_t p_hash, HashTableElement<TKey, TValue> *p_value, uint32_t p_table_size) {
		// Open hashing --> closed hashing allows us to keep the initial hash, useful for comparison
		uint32_t idx = p_hash % p_table_size;
		int probe_len = 0;
		HashTableElement<TKey, TValue> *data = p_value;
		uint32_t hash = p_hash;

		while (true) {
			// Found empty hash
			if (hashes[idx] == 0) {
				hashes[idx] = hash;
				hashed_data[idx] = data;
				element_count++;
				return;
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

	/**
	 * @brief Internal insertion method. Takes the precalculated hash and a reference to the key-value pair to insert.
	 * @param p_hash The pre-calculated hash to use as a reference for where to insert the data. This value is much
	 * larger than the table size (usually) and as such we mod it against table size to find a location.
	 * @param p_value The key-value pair corresponding to a respective key and value to insert. This object is
	 * re-constructed when allocating the `HashTableElement`.
	 * @return A pointer to the `HashTableElement` that was just inserted into the `HashTable`. This pointer is valid
	 * under permutations - it is only freed when the `erase` method is called on the given key, or when the entire
	 * `HashTable` is cleared. This makes the table safer to use data-wise, but slower to recalculate.
	 */
	HashTableElement<TKey, TValue> *_insert(uint32_t p_hash, const KeyValue<TKey, TValue> &p_value) {
		uint32_t size = PRIMES[_prime_idx];

		// Need to allocate table
		if (hashed_data == nullptr) {
			hashed_data = (HashTableElement<TKey, TValue> **)Memory::vallocate_zeroed(
				size * sizeof(HashTableElement<TKey, TValue> *));
			hashes = (uint32_t *)Memory::vallocate_zeroed(size * sizeof(uint32_t));
		}

		// Check if a resize is needed (most of the table is now being occupied)
		if (element_count + 1 > 0.8 * size) {
			_resize_and_remap(size, PRIMES[++_prime_idx]);
			// Modify size since it's just changed
			size = PRIMES[_prime_idx];
		}

		HashTableElement<TKey, TValue> *elem =
			Allocator::new_allocation(HashTableElement<TKey, TValue>(p_value.key, p_value.value));

		if (_tail == nullptr) {
			_head = elem;
			_tail = elem;
		} else {
			_tail->next = elem;
			elem->prev = _tail;
			_tail = elem;
		}

		// Insert new element
		_insert_element(p_hash, elem, size);
		return elem;
	}

public:
	/**
	 * @brief Checks to see if the number of elements in the `HashTable` is zero.
	 * @return `true` if yes, `false` if not.
	 */
	FORCE_INLINE bool is_empty() const {
		return element_count == 0;
	}

	/**
	 * @brief Obtains the number of elements present in the `HashTable`. This value is not the same as its capacity, as
	 * a `HashTable` reallocates its contents when a certain number of elements has been allocated, since re-hashing
	 * all of its contents on every insertion would get pointlessly expensive.
	 * @return The number of elements in the `HashTable`.
	 */
	FORCE_INLINE int64_t size() const {
		return element_count;
	}

	/**
	 * @brief Obtains the capacity of the `HashTable`. This value represents the number of values that can currently be
	 * fit inside of the `HashTable` before it runs out of space. Note that resizing and rehashing occurs at (elements
	 * + 1 > 0.8 * capacity) instead of at max capacity to hopefully reduce the number of collisions. Additionally, the
	 * capacity of the `HashTable` is always a prime number (see `PRIMES` as defined in `hashfuncs.cpp`).
	 * @return The capacity of the `HashTable`.
	 */
	FORCE_INLINE int64_t get_capacity() const {
		return PRIMES[_prime_idx];
	}

	/* Iterators */

	// Constant iterator for constant usages
	struct ConstIterator {
		FORCE_INLINE const KeyValue<TKey, TValue> &operator*() const {
			return e->data;
		}
		FORCE_INLINE const KeyValue<TKey, TValue> *operator->() const {
			return &e->data;
		}

		FORCE_INLINE ConstIterator &operator++() {
			e = e->next;
			return *this;
		}

		FORCE_INLINE ConstIterator &operator--() {
			e = e->prev;
			return *this;
		}

		FORCE_INLINE bool operator==(const ConstIterator &p_other) const {
			return e == p_other.e;
		}
		FORCE_INLINE bool operator!=(const ConstIterator &p_other) const {
			return e != p_other.e;
		}

		ConstIterator(const HashTableElement<TKey, TValue> *p_E) {
			e = p_E;
		}
		ConstIterator() {}
		ConstIterator(const ConstIterator &p_iter) {
			e = p_iter.e;
		}

	private:
		const HashTableElement<TKey, TValue> *e = nullptr;
	};

	// Constructs an `ConstIterator` from the first element in the list.
	FORCE_INLINE ConstIterator begin() const {
		return ConstIterator(_head);
	}

	// Constructs an `ConstIterator` from the last element in the list (always nullptr)
	FORCE_INLINE ConstIterator end() const {
		return ConstIterator(nullptr);
	}

	// Iterator struct for non-constant usages
	struct Iterator {
		FORCE_INLINE KeyValue<TKey, TValue> &operator*() const {
			return e->data;
		}
		FORCE_INLINE KeyValue<TKey, TValue> *operator->() const {
			return &e->data;
		}

		FORCE_INLINE Iterator &operator++() {
			if (e) {
				e = e->next;
			}
			return *this;
		}

		FORCE_INLINE Iterator &operator--() {
			if (e) {
				e = e->prev();
			}
			return *this;
		}

		FORCE_INLINE bool operator==(const Iterator &p_other) const {
			return e == p_other.e;
		}
		FORCE_INLINE bool operator!=(const Iterator &p_other) const {
			return e != p_other.e;
		}

		Iterator(HashTableElement<TKey, TValue> *p_E) {
			e = p_E;
		}
		Iterator() {}
		Iterator(const Iterator &p_iter) {
			e = p_iter.e;
		}

	private:
		HashTableElement<TKey, TValue> *e = nullptr;
	};

	// Constructs an `Iterator` from the first element in the list.
	FORCE_INLINE Iterator begin() {
		return Iterator(_head);
	}

	// Constructs an `Iterator` from the last element in the list (always nullptr)
	FORCE_INLINE Iterator end() {
		return Iterator(nullptr);
	}

	/* Getters and setters */

	/**
	 * @brief Obtains a reference to the value that is held by `p_key`. If the key does not exist in the `HashTable`,
	 * it crashes the engine as it expects the user to have inserted data for that given key beforehand. This method
	 * differs from `operator[]` as the operator is likely being used as a shorthand for data insertion, and therefore
	 * it makes sense for it to insert data. In this case, the engine should crash as the user has attempted to insert
	 * data without using an appropriate method.
	 * @param p_key The key to find the corresponding value of.
	 * @return A reference to the value held by `p_key`.
	 */
	FORCE_INLINE TValue &get(const TKey &p_key) {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		CRASH_COND_MSG(!exists, "HashTable key could not be found.");
		return hashed_data[idx]->data.value;
	}

	/**
	 * @brief Obtains a constant reference to the value that is held by `p_key`. If the key does not exist in the
	 * `HashTable`, it crashes the engine as it expects the user to have inserted data for the key prior to usage, or
	 * to have obtained a non-constant reference. To check if the `HashTable` has the key beforehand, use the `has`
	 * method.
	 * @param p_key The key to find the corresponding value of.
	 * @return A constant reference to the value held by `p_key`.
	 */
	FORCE_INLINE const TValue &get(const TKey &p_key) const {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		CRASH_COND_MSG(!exists, "HashTable key could not be found.");
		return hashed_data[idx]->data.value;
	}

	/**
	 * @brief Obtains a pointer to the value held by the given `HashTable`. If the value does not exist, returns
	 * `nullptr`.
	 * @param p_key The key to find the data for.
	 * @return A pointer to the value held by the data, or `nullptr` if the key does not exist in the `HashTable`.
	 */
	FORCE_INLINE TValue *get_ptr(const TKey &p_key) {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		if (exists) {
			return &hashed_data[idx]->data.value;
		}

		return nullptr;
	}

	/**
	 * @brief Obtains a constant pointer to the value held by the given `HashTable`. If the value does not exist,
	 * returns `nullptr`.
	 * @param p_key The key to find the data for.
	 * @return A constant pointer to the value held by the data, or `nullptr` if the key does not exist in the
	 * `HashTable`.
	 */
	FORCE_INLINE const TValue *get_ptr(const TKey &p_key) const {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		if (exists) {
			return &hashed_data[idx]->data.value;
		}

		return nullptr;
	}

	/**
	 * @brief Obtains a reference to the value held by the given key. If the value does not exist, it inserts a
	 * key-value pair for you with the default for the value set.
	 * @param p_key The key to find the data for.
	 * @return A reference to the data in the `HashTable`.
	 */
	FORCE_INLINE TValue &operator[](const TKey &p_key) {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		if (!exists) {
			return _insert(h, KeyValue<TKey, TValue>(p_key, TValue()))->data.value;
		}

		return hashed_data[idx]->data.value;
	}

	/**
	 * @brief Obtains a constant reference to the value held by the given key. If the value does not exist, then the
	 * engine crashes as it expects the user to have inputted the data first, or to have obtained a non-constant
	 * reference instead. To check for this beforehand, use the `has` method.
	 * @param p_key The key to check for the data of.
	 * @return A constant reference to the data in the `HashTable`.
	 */
	FORCE_INLINE const TValue &operator[](const TKey &p_key) const {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		CRASH_COND_MSG(!exists, "HashTable key could not be found.");
		return hashed_data[idx]->data.value;
	}

	/**
	 * @brief Checks to see if the given key exists inside of the `HashTable`. Useful for checking whether a value can
	 * be obtained prior to getting its reference.
	 * @param p_key The key to check as to whether it is valid.
	 * @return `true` if the key leads to valid data, `false` if not. If this method returns `false`, attempting to use
	 * the `get` functions will crash the engine. Use `operator[]` or `insert` if you wish to insert data.
	 */
	FORCE_INLINE bool has(const TKey &p_key) const {
		uint32_t h = _hash(p_key);
		uint32_t tmp;
		return _find_hashed_index(p_key, h, tmp);
	}

	/**
	 * @brief Inserts a key-value pair into the `HashTable`. Hashes the key for you, how kind.
	 * @param p_key The key of the pair.
	 * @param p_value The value of the pair.
	 */
	FORCE_INLINE void insert(const TKey &p_key, const TValue &p_value) {
		uint32_t h = _hash(p_key);
		_insert(h, KeyValue<TKey, TValue>(p_key, p_value));
	}

	/**
	 * @brief Removes the given entry from the `HashTable`, if it exists. Any pointers looking at this entry will now
	 * become invalid and will not work.
	 * @param p_key The key corresponding to the entry to remove.
	 * @return `true` if the entry exists and was deleted, `false` if it did not exist. Data being nonexistent is not
	 * considered an error and as such no error message will be emitted.
	 */
	FORCE_INLINE bool erase(const TKey &p_key) {
		uint32_t h = _hash(p_key);
		uint32_t idx = 0;
		bool exists = _find_hashed_index(p_key, h, idx);
		if (!exists) {
			return false;
		}

		// Removing an item should mean that every item with the same hash is moved down a step
		// Check hash idx + 1
		// If hash is valid, move down. Invalidate hash idx + 1
		// Repeat at idx + 2
		uint32_t next_idx = idx + 1;
		uint32_t size = PRIMES[_prime_idx];
		while (hashes[next_idx] != 0) {
			// Get closed hash (should point to next index)
			uint32_t h2 = hashes[next_idx] % size;
			if (h2 == idx) {
				SWAP(hashed_data[next_idx - 1], hashed_data[next_idx]);
			} else {
				break;
			}

			_inc_mod(next_idx, size);
		}

		hashes[idx] = 0;
		if (_head == hashed_data[idx]) {
			_head = hashed_data[idx]->next;
		}

		if (_tail == hashed_data[idx]) {
			_tail = hashed_data[idx]->prev;
		}

		if (hashed_data[idx]->prev) {
			hashed_data[idx]->prev->next = hashed_data[idx]->next;
		}

		if (hashed_data[idx]->next) {
			hashed_data[idx]->next->prev = hashed_data[idx]->prev;
		}

		Allocator::delete_allocation(hashed_data[idx]);

		element_count--;
		return true;
	}

	/**
	 * @brief Clears any data currently held by the `HashTable`. Does not free the pointers, since memory is still
	 * reserved for use until the `HashTable` goes out of scope.
	 */
	FORCE_INLINE void clear() {
		HashTableElement<TKey, TValue> *current = _tail;
		while (current != nullptr) {
			HashTableElement<TKey, TValue> *prev = current->prev;
			Allocator::delete_allocation(current);
			current = prev;
		}
		Memory::vzero(hashes, sizeof(uint32_t) * element_count);
		_tail = nullptr;
		_head = nullptr;
		element_count = 0;
	}

	/**
	 * @brief Sets the initial capacity for the given `HashTable`. The value inputted is always rounded up to the
	 * nearest prime number, which for large values may be quite larger than expected, so be careful when using this
	 * function.
	 * @param p_capacity The initial capacity of the `HashTable`.
	 */
	void reserve(uint32_t p_capacity) {
		uint32_t nprime = _prime_idx;

		while (PRIMES[nprime] < p_capacity) {
			ERR_FAIL_COND(nprime + 1 >= PRIMES_SIZE);
			nprime++;
		}

		if (nprime == _prime_idx) {
			if (p_capacity < element_count) {
				ERR_WARN("Attempted to reserve less memory than what currently exists.");
			}
			return;
		}

		if (hashed_data == nullptr) {
			_prime_idx = nprime;
			return;
		}

		_resize_and_remap(element_count, PRIMES[nprime]);
	}

	/* Assignment operators */

	/**
	 * @brief Assignment operator. Copies data over to the current `HashTable`. Both tables retain unique memory.
	 * @param p_other The `HashTable` to copy data from.
	 */
	void operator=(const HashTable &p_other) {
		if (this == &p_other) {
			return;
		}

		if (element_count > 0) {
			clear();
		}

		reserve(p_other._prime_idx);

		if (p_other.hashed_data == nullptr) {
			return;
		}

		for (const KeyValue<TKey, TValue> &kv : p_other) {
			insert(kv.key, kv.value);
		}
	}

	/**
	 * @brief Assignment operator. Moves data from the given `HashTable` to this one, and clears the data in the other
	 * table. Obtains ownership of any data held.
	 * @param p_from The `HashTable` to move data from.
	 */
	void operator=(HashTable &&p_from) {
		if (this == &p_from) {
			return;
		}

		if (element_count != 0) {
			clear();
		}

		if (hashed_data != nullptr) {
			Memory::vfree(hashed_data);
			Memory::vfree(hashes);
		}

		hashed_data = p_from.hashed_data;
		hashes = p_from.hashes;
		_head = p_from._head;
		_tail = p_from._tail;
		_prime_idx = p_from._prime_idx;
		element_count = p_from.element_count;

		p_from.hashed_data = nullptr;
		p_from.hashes = nullptr;
		p_from._head = nullptr;
		p_from._tail = nullptr;
		p_from._prime_idx = 2;
		p_from.element_count = 0;
	}

	/* Constructors */

	/**
	 * @brief Constructs a `HashTable` with the capacity set to the lowest possible non-zero prime.
	 */
	HashTable() {
		_prime_idx = 0;
	}

	/**
	 * @brief Constructs a `HashTable` with an initial capacity. The capacity is always rounded up to the nearest prime
	 * number (see `PRIMES` in `hashfuncs.cpp`).
	 * @param p_initial_capacity The initial capacity of the `HashTable`.
	 */
	HashTable(uint64_t p_initial_capacity) {
		_prime_idx = 0;
		reserve(p_initial_capacity);
	}

	/**
	 * @brief Constructs a `HashTable` from an existing `HashTable` of the same type by copying the information over
	 * from the previous table. The call must be explicit (i.e. we cannot do `HashTable<K, V> h2 = h` and must do
	 * `HashTable<K, V> h2(h)` or `HashTable<K, V> h2 = HashTable<K, V>(h)` instead.)
	 * @param p_other The `HashTable` to copy data from.
	 */
	explicit HashTable(const HashTable &p_other) {
		reserve(PRIMES[p_other._prime_idx]);

		if (p_other.element_count == 0) {
			return;
		}

		for (const KeyValue<TKey, TValue> &kv : p_other) {
			insert(kv.key, kv.value);
		}
	}

	/**
	 * @brief Constructs a `HashTable` by moving the contents of another table into this one. Invoked by the
	 * `std::move()` operator which should be available in all code. Since the other table is likely going
	 * out of scope, it sets all the data in it to the default values and takes ownership of the pointers.
	 * @param p_other The `HashTable` to move data from into here.
	 */
	HashTable(HashTable &&p_other) {
		hashed_data = p_other.hashed_data;
		hashes = p_other.hashes;
		_head = p_other._head;
		_tail = p_other._tail;
		_prime_idx = p_other._prime_idx;
		element_count = p_other.element_count;

		p_other.hashed_data = nullptr;
		p_other.hashes = nullptr;
		p_other._head = nullptr;
		p_other._tail = nullptr;
		p_other._prime_idx = 2;
		p_other.element_count = 0;
	}

	/**
	 * @brief Constructs a `HashTable` from a static initializer list. Useful for our test suite as we can define
	 * key/value pairs without having to append them (since appending is a separate operation we should check for), and
	 * possibly useful elsewhere in code.
	 * @param p_init The initializer list to construct data for.
	 */
	HashTable(std::initializer_list<KeyValue<TKey, TValue>> p_init) {
		reserve(p_init.size());
		for (const KeyValue<TKey, TValue> &kv : p_init) {
			insert(kv.key, kv.value);
		}
	}

	/**
	 * @brief Destroys the `HashTable`. Deletes any allocations made and frees the hashes and hashed data.
	 */
	~HashTable() {
		clear();
		if (hashed_data != nullptr) {
			Memory::vfree(hashed_data);
			Memory::vfree(hashes);
		}
	}
};
