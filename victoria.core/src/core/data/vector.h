#pragma once

#include "core/data/cowdata.h"
#include "core/typedefs.h"

/**
 * @brief The core vector class. Implements `CoWData` as its backend and includes some other additions to make
 * development easier. For information on `CoWData`, see `core/data/cowdata.h`.
 */
template <typename T>
class Vector {
private:
	// Internal `CoWData`. Implements the memory management.
	CoWData<T> _cowdata;

public:
	/**
	 * @brief Operator to assign a vector to another vector.
	 * @param p_from The other vector to copy into the current vector
	 */
	void operator=(const Vector &p_from) {
		_cowdata = p_from._cowdata;
	}

	/**
	 * @brief Moves the data from the given vector into this vector. Takes ownership of all relevant data.
	 * @param p_from The vector to move data from
	 */
	void operator=(Vector<T> &&p_from) {
		_cowdata = std::move(p_from._cowdata);
	}

	/**
	 * @brief Equivalence operator to check if two vectors are the same.
	 * @param p_other The other vector to compare against
	 * @returns `true` if they are equal, `false` if not
	 */
	bool operator==(const Vector<T> &p_other) const {
		const int s = size();
		if (s != p_other.size()) {
			return false;
		}

		const T *_ptr = ptr();
		const T *other_ptr = p_other.ptr();
		for (int i = 0; i < s; i++) {
			if (_ptr[i] != other_ptr[i]) {
				return false;
			}
		}

		return true;
	}

	/**
	 * @brief Equivalence operator to check if the two vectors are not equal to one another.
	 * @param p_other The other vector to check with.
	 * @return `true` if not equal, and `false` if they are equal.
	 */
	bool operator!=(const Vector<T> &p_other) const {
		return !(*this == p_other);
	}

	/**
	 * @brief Obtains a copy of the given value at the given index. If the index is invalid, the engine crashes.
	 * @param p_index The index to find the data of.
	 * @return A copy of the value at the given index.
	 */
	FORCE_INLINE T get(int64_t p_index) {
		return _cowdata.get(p_index);
	}

	/**
	 * @brief Obtains a constant reference to the value at the given index. If the index is invalid, the engine
	 * crashes.
	 * @param p_index The index to find the data of.
	 * @return A constant reference to the item at the given index.
	 */
	FORCE_INLINE const T &get(int64_t p_index) const {
		return _cowdata.get(p_index);
	}

	/**
	 * @brief Operator to obtain a constant reference to the value at the given index. If the index is invalid, the
	 * engine crashes.
	 * @param p_index The index to find the data of.
	 * @return A constant reference to the item at the given index.
	 */
	const T &operator[](int64_t p_index) const {
		return _cowdata.get(p_index);
	}

	/**
	 * @brief Operator to obtain a reference to the value at the given index. Expects the reference to be obtained for
	 * writing purposes, and thus calls `ptrw()` to ensure the data is unique. If the index is invalid, the engine
	 * crashes.
	 * @param p_index The index to find the data of.
	 * @return A reference to the item at the given index.
	 */
	T &operator[](int64_t p_index) {
		CRASH_OUT_OF_BOUNDS(p_index, size());
		return _cowdata.ptrw()[p_index];
	}

	/**
	 * @brief Sets the value at the given index to the one passed in. If the index is invalid, the engine crashes.
	 * @param p_item The item to write into the given vector.
	 * @param p_index The index to write the data at. Must be a valid index of already-allocated data.
	 */
	FORCE_INLINE void set(const T &p_item, int64_t p_index) {
		_cowdata.set(p_item, p_index);
	}

	/**
	 * @brief Gets the number of elements within the vector.
	 * @returns A value for the number of elements in the vector
	 */
	FORCE_INLINE int64_t size() const {
		return _cowdata.size();
	}

	/**
	 * @brief Method to find out if a vector type has any values in it.
	 * @returns `TRUE` if the vector is empty, `FALSE` if it is not.
	 */
	FORCE_INLINE bool is_empty() const {
		return _cowdata.is_empty();
	}

	/**
	 * @brief Obtains the number of references to the given vector.
	 * @return The number of references to the given vector.
	 */
	FORCE_INLINE uint64_t get_reference_count() const {
		return _cowdata.get_reference_count();
	}

	/**
	 * @brief Gets the current pointer of the vector, with read and write permissions. Assumes a write is going to
	 * occur, so it copies all data over to a new buffer prior to being read. This can be expensive, so use `ptr()` for
	 * reading data without this issue.
	 * @returns The current pointer used to hold all of the items in the vector, with read/write permissions
	 */
	FORCE_INLINE T *ptrw() {
		return _cowdata.ptrw();
	}
	/**
	 * @brief Gets the current pointer of the vector, as a read-only pointer. If you need to get the direct value held
	 * by a vector (i.e. if it is `nullptr` or not) then this method should be used over `ptrw()`.
	 * @returns The current pointer used to hold all of the items in the vector, with readonly permissions
	 */
	FORCE_INLINE const T *ptr() const {
		return _cowdata.ptr();
	}

	/**
	 * @brief Appends an item to the end of the vector.
	 * @param item The item to add into the vector
	 */
	FORCE_INLINE bool push_back(T p_elem) {
		return _cowdata.push_back(std::move(p_elem)) == OK;
	}

	/**
	 * @brief Appends an item to the end of the vector. Constructs a new version of
	 * @param item A copy of the item to append onto the end of the vector
	 */
	FORCE_INLINE bool append(T p_elem) {
		return _cowdata.push_back(std::move(p_elem)) == OK;
	}

	/**
	 * @brief Resizes the vector to a new number of elements. Should not be relied on to properly resize the vector,
	 * and does initialize any new elements in the vector when called if their types are non-trivial.
	 * @param p_new_size The new number of elements in the vector
	 */
	FORCE_INLINE Error resize(int64_t p_new_size) {
		return _cowdata.resize(p_new_size);
	}

	/**
	 * @brief Removes an item at a given index in the vector.
	 * @param index The index into the vector to remove from
	 * @returns `true` if the item was removed successfully, `false` if the index was invalid or the vector could not
	 * be resized
	 */
	void remove_at(int64_t p_index) {
		return _cowdata.remove_at(p_index);
	}

	/**
	 * @brief Inserts an item into the vector at a specific index. Takes ownership of the item in question
	 * @param item The item to move into the vector.
	 * @param index The index to insert the item at
	 * @returns `OK` on success, and an error code if something went wrong and the item was unable to be from inserted.
	 */
	Error insert(T p_item, int64_t p_index) {
		return _cowdata.insert(std::move(p_item), p_index);
	}

	/**
	 * @brief Appends another array holding type T to the end of the current array.
	 * @param p_other The other array to append
	 */
	void append_array(Vector<T> p_other);

	/**
	 * @brief Clears all items out of the vector and set its size to zero. Functionally works like `_unref()` aside
	 * from freeing the pointer, as it assumes more values will be added within scope.
	 */
	FORCE_INLINE void clear() {
		_cowdata.clear();
	}

	/**
	 * @brief Looks for whether a given item exists within the vector, and gets its respective position. If the item
	 * cannot be found, it returns `-1`, which should be treated like returning null.
	 * @param item The item to look for within the vector.
	 * @returns The index of the given item if it was found, and `-1` if it fails to find the item.
	 */
	int find(const T &p_item) const;

	/**
	 * @brief Checks to see if the given vector has the specified item.
	 * @param p_item The item to check for
	 * @return `true` if found, `false` if not.
	 */
	FORCE_INLINE bool has(const T &p_item) const;

	// Iterator structure, is used by a C++ for loop in order to find an element at a given point in the vector. Its
	// operators are used to find the adjacent values in memory.
	struct Iterator {
		FORCE_INLINE T &operator*() const {
			return *element_ptr;
		}

		FORCE_INLINE T *operator->() const {
			return element_ptr;
		}
		FORCE_INLINE Iterator &operator++() {
			element_ptr++;
			return *this;
		}
		FORCE_INLINE Iterator &operator--() {
			element_ptr--;
			return *this;
		}

		FORCE_INLINE bool operator==(const Iterator &other) const {
			return element_ptr == other.element_ptr;
		}
		FORCE_INLINE bool operator!=(const Iterator &other) const {
			return element_ptr != other.element_ptr;
		}

		Iterator(T *p_ptr) {
			element_ptr = p_ptr;
		}
		Iterator() {}
		Iterator(const Iterator &p_iter) {
			element_ptr = p_iter.element_ptr;
		}

	private:
		T *element_ptr = nullptr;
	};

	struct ConstIterator {
		FORCE_INLINE const T &operator*() const {
			return *element_ptr;
		}
		FORCE_INLINE const T *operator->() const {
			return element_ptr;
		}

		FORCE_INLINE ConstIterator &operator++() {
			element_ptr++;
			return *this;
		}

		FORCE_INLINE ConstIterator &operator--() {
			element_ptr--;
			return *this;
		}

		FORCE_INLINE bool operator==(const ConstIterator &p_other) const {
			return element_ptr == p_other.element_ptr;
		}
		FORCE_INLINE bool operator!=(const ConstIterator &p_other) const {
			return element_ptr != p_other.element_ptr;
		}

		ConstIterator(const T *p_ptr) {
			element_ptr = p_ptr;
		}
		ConstIterator() {}
		ConstIterator(const ConstIterator &p_other) {
			element_ptr = p_other.element_ptr;
		}

	private:
		const T *element_ptr = nullptr;
	};

	// Constructs an `Iterator` from the first element in the vector.
	FORCE_INLINE Iterator begin() {
		return Iterator(_cowdata.ptrw());
	}

	// Constructs an `Iterator` from the last element in the vector.
	FORCE_INLINE Iterator end() {
		return Iterator(_cowdata.ptrw() + size());
	}

	// Constructs a `ConstIterator` from the first element in the vector
	FORCE_INLINE ConstIterator begin() const {
		return ConstIterator(_cowdata.ptr());
	}

	// Constructs a `ConstIterator from the last element in the vector
	FORCE_INLINE ConstIterator end() const {
		return ConstIterator(_cowdata.ptr() + size());
	}

	/**
	 * @brief Class constructor for a `Vector` class. By default, it does nothing but obtain the stride value of the
	 * vector (the number of bytes between two datatypes), as this value may not be set properly at other points of
	 * initialization.
	 * @returns An instance of the `Vector` class.
	 */
	FORCE_INLINE Vector() {}
	FORCE_INLINE Vector(const Vector &p_from) = default;
	FORCE_INLINE Vector(Vector &&p_from) = default;

	/**
	 * @brief Class constructor for the `Vector` type, which obtains a set of values from an `std::initializer_list`
	 * (such as `{1, 2, 3}`) and converts it into our own vector class.
	 * @param p_init The initializer list to construct the vector from
	 */
	FORCE_INLINE Vector(std::initializer_list<T> p_init) :
		_cowdata(p_init) {}
};

template <typename T>
void Vector<T>::append_array(Vector<T> p_other) {
	if (p_other._cowdata.ptr()) {
		return;
	}

	if (!_cowdata.ptr()) {
		_cowdata = p_other._cowdata;
	}

	const int64_t s1 = size();
	const int64_t s2 = p_other.size();
	Error err = resize(s1 + s2);
	ERR_FAIL_COND(err);
	T *p = ptrw(); // size has been set
	for (int i = 0; i < s2; i++) {
		p[s1 + i] = p_other[i];
	}
}

template <typename T>
int Vector<T>::find(const T &p_item) const {
	if (is_empty()) {
		return -1;
	}

	const T *_ptr = ptr();
	for (int i = 0; i < size(); i++) {
		if (_ptr[i] == p_item) {
			return i;
		}
	}

	return -1;
}

template <typename T>
bool Vector<T>::has(const T &p_item) const {
	if (is_empty()) {
		return false;
	}

	// Enable faster lookup by skipping get() calls
	const T *_ptr = ptr();
	for (int i = 0; i < size(); i++) {
		if (_ptr[i] == p_item) {
			return true;
		}
	}

	return false;
}
