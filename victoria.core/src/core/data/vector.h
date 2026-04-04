#pragma once

#include "atomic_counter.h"

#include "core/error/error_macros.h"
#include "core/os/memory.h"
#include "core/typedefs.h"

/**
 * @brief Our core vector class. Acts as an implementation of `std::vector`, which we use over the latter to keep our
 * code lightweight and readable for end-users. This class consists of one pointer which acts as the raw vector to
 * access and retrieve values from, and several `uint64_t`s that keep count of its size, elements and the standalone
 * data size. For situations where one may alternate between datatypes, please refer to the `Array` class, which can be
 * found under `core/ variant/array.h`.
 */
template <typename T>
class Vector {
private:
	friend class String;

	// The refcount offset pointer.
	static constexpr size_t REFC_OFFSET = 0;
	static constexpr size_t SIZE_OFFSET = REFC_OFFSET + sizeof(AtomicCounter<uint64_t>);
	static constexpr size_t DATA_OFFSET = SIZE_OFFSET + sizeof(uint64_t);

	// The internal pointer that makes up the actual array in memory
	mutable T *_ptr = nullptr;

	/**
	 * @brief Gets the reference count from the pointer. Assumes the pointer is not null.
	 * @return A pointer to the reference count object.
	 */
	FORCE_INLINE AtomicCounter<uint64_t> *_get_refc() const {
		return (AtomicCounter<uint64_t> *)(((uint8_t *)_ptr) - DATA_OFFSET + REFC_OFFSET);
	}

	/**
	 * @brief Gets the element count from the pointer. Assumes the pointer is not null.
	 * @return A pointer to the element count.
	 */
	FORCE_INLINE uint64_t *_get_size() const {
		return (uint64_t *)(((uint8_t *)_ptr) - DATA_OFFSET + SIZE_OFFSET);
	}

	FORCE_INLINE void _ref(const Vector &p_from);
	FORCE_INLINE void _unref();
	FORCE_INLINE void _copy_on_write();
	FORCE_INLINE Error _resize(int64_t n_size);

	FORCE_INLINE Error _copy_to_new_buffer(uint64_t p_size);
	FORCE_INLINE Error _alloc_buffer(uint64_t p_size);
	FORCE_INLINE Error _realloc_buffer(uint64_t p_size);

public:
	/**
	 * @brief Gets the given item from a given index into the vector.
	 * @param p_index The index into the vector we want to retrieve from
	 * @returns An item of type `T`
	 */
	FORCE_INLINE T &get(int64_t p_index) {
		ERR_OUT_OF_BOUNDS_FATAL(p_index, size());
		_copy_on_write();
		return _ptr[p_index];
	}

	/**
	 * @brief Gets the given item from a given index into the vector.
	 * @param p_index The index into the vector we want to retrieve from
	 * @returns An item of type `T`
	 */
	FORCE_INLINE const T &get(int64_t p_index) const {
		ERR_OUT_OF_BOUNDS_FATAL(p_index, size());
		return _ptr[p_index];
	}

	/**
	 * @brief Sets the item at a given index to a specific value.
	 * @param p_index The given index to set it at
	 * @param item The given item to set
	 */
	FORCE_INLINE void set(const T &item, int64_t p_index) {
		ERR_OUT_OF_BOUNDS_FATAL(p_index, size());
		// Since the data is set up so that data isn't unique - not normally a problem for basic (atomic) data, but for
		// Strings (data types with a nested vector) it has a whole load of problems. To fix this, we need to copy over
		// data properly, into the block itself
		_copy_on_write();
		_ptr[p_index] = item;
	}

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

	// Constructs an `Iterator` from the first element in the vector.
	FORCE_INLINE Iterator begin() {
		return Iterator(_ptr);
	}

	// Constructs an `Iterator` from the last element in the vector.
	FORCE_INLINE Iterator end() {
		return Iterator(_ptr + size());
	}

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

	// Constructs a `ConstIterator` from the first element in the vector
	FORCE_INLINE ConstIterator begin() const {
		return ConstIterator(_ptr);
	}

	// Constructs a `ConstIterator from the last element in the vector
	FORCE_INLINE ConstIterator end() const {
		return ConstIterator(_ptr + size());
	}

	/**
	 * @brief Operator to access an item at the given index.
	 * @param index The index in the vector to access
	 * @returns The item at the given index
	 */
	FORCE_INLINE T &operator[](int64_t index) {
		// Check for an out of bounds index and throw an error if so
		ERR_OUT_OF_BOUNDS_FATAL(index, size());
		_copy_on_write();
		return ((Vector<T> *)(this))->_ptr[index];
	}
	/**
	 * @brief Operator to access the item at the given index.
	 * @param index The index in the vector to access
	 * @returns The item at the given index
	 */
	FORCE_INLINE const T &operator[](int64_t index) const {
		ERR_OUT_OF_BOUNDS_FATAL(index, size());

		return _ptr[index];
	}

	/**
	 * @brief Operator to assign a vector to another vector.
	 * @param p_from The other vector to copy into the current vector
	 */
	inline void operator=(const Vector &p_from) {
		_ref(p_from);
	}

	FORCE_INLINE void operator=(Vector<T> &&p_from) {
		if (_ptr == p_from._ptr) {
			return;
		}

		_unref();
		_ptr = p_from._ptr;
		p_from._ptr = nullptr;
	}

	FORCE_INLINE bool operator==(const Vector &p_other) const;

	FORCE_INLINE bool operator!=(const Vector &p_other) const;

	/**
	 * @brief Gets the number of elements within the vector.
	 * @returns A value for the number of elements in the vector
	 */
	FORCE_INLINE int64_t size() const {
		return _ptr ? *_get_size() : 0;
	};
	/**
	 * @brief Method to find out if a vector type has any values in it.
	 * @returns `TRUE` if the vector is empty, `FALSE` if it is not.
	 */
	FORCE_INLINE bool is_empty() const {
		return _ptr == nullptr || *_get_size() == 0;
	};
	/**
	 * @brief Gets the number of bytes the vector currently has allocated to itself.
	 * @returns The size of the vector in bytes
	 */
	FORCE_INLINE uint64_t get_ptr_size() const {
		return _ptr != nullptr ? *_get_size() * sizeof(T) : 0;
	};

	FORCE_INLINE uint64_t get_reference_count() const {
		return _ptr ? _get_refc()->get() : 0;
	}

	/**
	 * @brief Gets the current pointer of the vector, with read and write permissions. It is strongly advised to not
	 * access this directly unless absolutely necessary, as any modification to the pointer will not update any other
	 * information it holds.
	 * @returns The current pointer used to hold all of the items in the vector, with read/write permissions
	 */
	FORCE_INLINE T *ptrw() {
		_copy_on_write();
		return _ptr;
	}
	/**
	 * @brief Gets the current pointer of the vector, as a read-only pointer. If you need to get the direct value held
	 * by a vector (i.e. if it is `nullptr` or not) then this method should be used over `ptrw()`.
	 * @returns The current pointer used to hold all of the items in the vector, with readonly permissions
	 */
	FORCE_INLINE const T *ptr() const {
		return _ptr;
	}

	/**
	 * @brief Resizes the vector to a new number of elements. Differs from `_resize` in that it automatically
	 * calculates the number of bytes per-element.
	 * @param p_new_size The new number of elements in the vector
	 */
	FORCE_INLINE Error resize(int64_t p_new_size) {
		return _resize(p_new_size * sizeof(T));
	}

	FORCE_INLINE void append(T item);
	FORCE_INLINE void append_array(Vector<T> p_other);
	FORCE_INLINE void remove_at(int64_t index);
	FORCE_INLINE void insert_at(const T &item, int64_t index);

	FORCE_INLINE void push_front(T item);
	FORCE_INLINE T pop_front();
	FORCE_INLINE void push_back(T item);
	FORCE_INLINE T pop_back();

	FORCE_INLINE void clear();

	FORCE_INLINE int find(const T &p_item) const;
	FORCE_INLINE bool has(const T &p_item) const;

	/**
	 * @brief Class constructor for a `Vector` class. By default, it does nothing but obtain the stride value of the
	 * vector (the number of bytes between two datatypes), as this value may not be set properly at other points of
	 * initialization.
	 * @returns An instance of the `Vector` class.
	 */
	FORCE_INLINE Vector() {}
	FORCE_INLINE Vector(const Vector &p_from);

	/**
	 * @brief Class constructor for the `Vector` type, which obtains a set of values from an `std::initializer_list`
	 * (such as `{1, 2, 3}`) and converts it into our own vector class.
	 * @param p_init The initializer list to construct the vector from
	 */
	FORCE_INLINE Vector(std::initializer_list<T> p_init) {
		ERR_FAIL_COND(_resize(p_init.size() * sizeof(T)) != OK);

		int i = 0;
		for (const T &element : p_init) {
			_ptr[i] = element;

			i++;
		}

		*_get_size() = i;
	}

	FORCE_INLINE ~Vector();
};

/**
 * @brief Private initializer method. Unreferences any data currently held, and copies the pointer to the incoming
 * vector. Does not fire if the two are the same.
 * @param p_from The other vector to copy data from
 */
template <typename T>
void Vector<T>::_ref(const Vector &p_from) {
	if (_ptr == p_from._ptr) {
		return;
	}

	_unref();

	if (!p_from._ptr) {
		return;
	}

	// Reference other vector by copying the pointer.
	if (p_from._get_refc()->conditional_increment() > 0) {
		_ptr = p_from._ptr;
	}
}

/**
 * @brief Frees all of the data currently held within the vector, and sets the pointer to null if we are said pointer's
 * only owner. If not, it sets this pointer to null and returns early, but does not free the data.
 */
template <typename T>
void Vector<T>::_unref() {
	if (!_ptr) {
		return;
	}

	// Data used is shared elsewhere in the application, don't free.
	if (_get_refc()->decrement() > 0) {
		_ptr = nullptr;
		return;
	}

	if (!std::is_trivially_destructible<T>::value) {
		int64_t current_size = size();

		for (int i = 0; i < current_size; i++) {
			_ptr[i].~T();
		}
	}

	// Set pointer to null and free a copy of it.
	T *prev = _ptr;
	_ptr = nullptr;
	Memory::vfree((((uint8_t *)prev) - DATA_OFFSET));
}

/**
 * @brief Copies the data to a new buffer, as it expects a new version of the vector is about to be created. Does not
 * copy if the number of references is one, since that data is not shared and can be left as-is.
 */
template <typename T>
void Vector<T>::_copy_on_write() {
	if (!_ptr || _get_refc()->get() == 1) {
		return;
	}

	_copy_to_new_buffer(size());
}

/**
 * @brief Changes the size of the current vector to a new given size.
 * @param n_size The new size of the vector in bytes
 * @return `OK` if the vector was able to be resized, and returns a given error message if something went wrong.
 */
template <typename T>
Error Vector<T>::_resize(int64_t n_size) {
	ERR_FAIL_COND_R(n_size < 0, ERR_INVALID_PARAMETER);

	int64_t current_size = get_ptr_size();
	uint64_t n_element_count = n_size / sizeof(T);

	if (current_size == n_size) {
		return OK; // no need for changes
	}

	if (current_size < n_size) {
		if (!_ptr) {
			Error err = _alloc_buffer(n_element_count);
			if (err) {
				return err;
			}
		} else if (_get_refc()->get() == 1) {
			Error err = _realloc_buffer(n_element_count);
			if (err) {
				return err;
			}
		} else {
			Error err = _copy_to_new_buffer(n_element_count);
			if (err) {
				return err;
			}
		}
	} else if (current_size > n_size) {
		// Destroy items that are no longer needed in the vector
		if (n_size == 0) {
			_unref();
			return OK;
		} else if (_get_refc()->get() == 1) {
			if (!std::is_trivially_destructible<T>::value) {
				for (uint64_t i = n_element_count; i < *_get_size(); i++) {
					_ptr[i].~T();
				}
			}

			Error err = _realloc_buffer(n_element_count);
			if (err) {
				return err;
			}
		} else {
			return _copy_to_new_buffer(n_element_count);
		}
	}
	return OK;
}

template <typename T>
Error Vector<T>::_copy_to_new_buffer(uint64_t p_count) {
	const Vector prev;
	prev._ptr = _ptr;
	_ptr = nullptr;

	Error err = _alloc_buffer(p_count);
	if (err) {
		_ptr = prev._ptr;
		prev._ptr = nullptr;
		return err;
	}

	memcpy_arr_placement(_ptr, prev._ptr, p_count);
	return OK;
}

/**
 * @brief Allocates a new pointer to the current buffer, as well as placing a new AtomicCounter in its slot and setting
 * the element count to the proper value.
 * @param p_element_count The new element count for the buffer.
 * @return `OK` on success, and `ERR_OUT_OF_MEMORY` if failed, which should crash.
 */
template <typename T>
Error Vector<T>::_alloc_buffer(uint64_t p_element_count) {
	T *ptr = (T *)Memory::vallocate((p_element_count * sizeof(T)) + DATA_OFFSET);
	ERR_COND_NULL_R(ptr, ERR_OUT_OF_MEMORY);

	_ptr = (T *)(((uint8_t *)ptr) + DATA_OFFSET);

	new (_get_refc()) AtomicCounter<uint64_t>(1);
	*_get_size() = p_element_count;
	return OK;
}

/**
 * @brief Reallocates the current buffer to a new size.
 * @param p_element_count The new number of elements in the buffer.
 * @return `OK` on success, and `ERR_OUT_OF_MEMORY` on failure, which should crash.
 */
template <typename T>
Error Vector<T>::_realloc_buffer(uint64_t p_element_count) {
	T *nptr = (T *)Memory::vreallocate(((uint8_t *)_ptr) - DATA_OFFSET, (p_element_count * sizeof(T)) + DATA_OFFSET);
	ERR_COND_NULL_R(nptr, ERR_OUT_OF_MEMORY);

	_ptr = (T *)(((uint8_t *)nptr) + DATA_OFFSET);
	ERR_FAIL_COND_R(_get_refc()->get() != 1, ERR_BUG);
	*_get_size() = p_element_count;
	return OK;
}

/**
 * @brief Equivalence operator to check if two vectors are the same.
 * @param p_other The other vector to compare against
 * @returns `true` if they are equal, `false` if not
 */
template <typename T>
bool Vector<T>::operator==(const Vector &p_other) const {
	const int s = size();
	if (s != p_other.size()) {
		return false;
	}

	for (int i = 0; i < s; i++) {
		if (_ptr[i] != p_other._ptr[i]) {
			return false;
		}
	}

	return true;
}

/**
 * @brief Equivalence operator to check if two vectors are not the same.
 * @param p_other The other vector to compare against
 * @returns `true` if they are not the same, and `false` if they are
 */
template <typename T>
bool Vector<T>::operator!=(const Vector &p_other) const {
	return !operator==(p_other);
}

/**
 * @brief Appends an item to the end of the vector. Acts the same as `push_back`.
 * @param item The item to append onto the end of the vector
 */
template <typename T>
void Vector<T>::append(T item) {
	push_back(item);
}

/**
 * @brief Appends another array holding type T to the end of the current array.
 * @param p_other The other array to append
 */
template <typename T>
void Vector<T>::append_array(Vector<T> p_other) {
	for (const T &p_item : p_other) {
		push_back(p_item);
	}
}

/**
 * @brief Removes an item at a given index in the vector.
 * @param index The index into the vector to remove from
 * @returns `true` if the item was removed successfully, `false` if the index was invalid or the vector could not be
 * resized
 */
template <typename T>
void Vector<T>::remove_at(int64_t index) {
	// Check if the index is out of bounds or not
	ERR_OUT_OF_BOUNDS(index, size());

	uint64_t old_size = size();

	if (old_size == 1) {
		_unref();
		return;
	}

	uint64_t new_size = old_size - 1;

	if (_get_refc()->get() == 1) {
		_ptr[index].~T();
		// Copy data down to the current pointer position.
		Memory::vmemmove(_ptr + index, _ptr + index + 1, (new_size - index) * sizeof(T));
		// Delete content at the end of the buffer
		ERR_FAIL_COND(_resize((*_get_size() - 1) * sizeof(T)) != OK);
	} else {
		// Copy data to a new buffer
		T *other = _ptr;
		_ptr = (T *)Memory::vallocate(new_size);
		if (std::is_trivially_copyable<T>::value) {
			Memory::vcopy_memory(_ptr, other, index * sizeof(T));
			Memory::vcopy_memory(_ptr + index, other + index, (new_size - index) * sizeof(T));
		} else {
			for (int i = 0; i < index; i++) {
				vnew_placement(_ptr + i, T(other[i]));
			}

			for (uint64_t i = index; i < new_size; i++) {
				vnew_placement(_ptr + i, T(other[i]));
			}
		}
	}
}

/**
 * @brief Inserts an item into the vector at a specific index.
 * @param item The item to insert
 * @param index The index to insert the item at
 * @returns `true` if successful, `false` if the vector could not be resized
 */
template <typename T>
void Vector<T>::insert_at(const T &item, int64_t index) {
	ERR_FAIL_COND(_resize(get_ptr_size() + sizeof(T)) != OK);
	for (int i = size() - 1; i > index; i--) {
		_ptr[i] = _ptr[i - 1];
	}

	_ptr[index] = item;
}

/**
 * @brief Appends an item to the front of the vector.
 * @param item The item to add into the vector
 */
template <typename T>
void Vector<T>::push_front(T item) {
	insert_at(item, 0);
}

/**
 * @brief Removes and gets the first item in the vector.
 * @returns The item at the front of the vector
 */
template <typename T>
T Vector<T>::pop_front() {
	T item = get(0);
	remove_at(0);
	return item;
}

/**
 * @brief Appends an item to the end of the vector.
 * @param item The item to add into the vector
 */
template <typename T>
void Vector<T>::push_back(T item) {
	Error err = _resize(get_ptr_size() + sizeof(T));
	ERR_FAIL_COND(err);
	vnew_placement(_ptr + size() - 1, T(std::move(item)));
}

/**
 * @brief Removes and gets the item at the end of the vector.
 * @returns The item at the end of the vector
 */
template <typename T>
T Vector<T>::pop_back() {
	int64_t end = *_get_size() - 1;
	T item = get(end);
	remove_at(end);
	return item;
}

/**
 * @brief Clears all items out of the vector and set its size to zero. Functionally works like `_unref()` aside from
 * freeing the pointer, as it assumes more values will be added within scope.
 */
template <typename T>
void Vector<T>::clear() {
	_resize(0);
}

/**
 * @brief Looks for whether a given item exists within the vector, and gets its respective position. If the item cannot
 * be found, it returns `-1`, which should be treated like returning null.
 * @param item The item to look for within the vector.
 * @returns The index of the given item if it was found, and `-1` if it fails to find the item.
 */
template <typename T>
int Vector<T>::find(const T &p_item) const {
	for (int i = 0; i < size(); i++) {
		if (_ptr[i] == p_item) {
			return i;
		}
	}

	return -1;
}

template <typename T>
bool Vector<T>::has(const T &p_item) const {
	for (int i = 0; i < size(); i++) {
		if (_ptr[i] == p_item) {
			return true;
		}
	}

	return false;
}

/**
 * @brief Class constructor for a new `Vector` type.
 */
template <typename T>
Vector<T>::Vector(const Vector &p_from) {
	_ref(p_from);
}

/**
 * @brief Class destructor for the `Vector` type.
 */
template <typename T>
Vector<T>::~Vector() {
	_unref();
}
