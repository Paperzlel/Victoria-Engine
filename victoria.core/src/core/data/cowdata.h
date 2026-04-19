#pragma once

#pragma once

#include "atomic_counter.h"

#include "core/error/error_macros.h"
#include "core/os/memory.h"
#include "core/typedefs.h"

/**
 * @brief A vector-like class that uses copy-on-write semantics for various operations, as well as taking ownership of
 * all data that is passed to it. For a class that is intended for end-users, see `core/data/vector.h`.
 */
template <typename T>
class CoWData {
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

	/**
	 * @brief Private initializer method. Unreferences any data currently held, and copies the pointer to the incoming
	 * vector. Does not fire if the two are the same.
	 * @param p_from The other vector to copy data from
	 */
	void _ref(const CoWData &p_from);

	/**
	 * @brief Frees all of the data currently held within the vector, and sets the pointer to null if we are said
	 * pointer's only owner. If not, it sets this pointer to null and returns early, but does not free the data.
	 */
	void _unref();

	/**
	 * @brief Copies the data to a new buffer, as it expects a new version of the vector is about to be created. Does
	 * not copy if the number of references is one, since that data is not shared and can be left as-is. Requires the
	 * result to be read as it pertains important info about the buffer.
	 */
	[[nodiscard]] Error _copy_on_write();

	/**
	 * @brief Copies data from the current buffer to a newly allocated one, by forking the current. Dereferences data
	 * by forcing the old pointer to go out of scope, which if it is the only one watching the data will cause a free.
	 * @param p_count The number of elements to copy from the old pointer.
	 * @return `OK` on success, and `ERR_OUT_OF_MEMORY` on failure, which is a likely crash condition.
	 */
	Error _copy_to_new_buffer(uint64_t p_old_count, uint64_t p_new_count);

	/**
	 * @brief Allocates a new pointer to the current buffer, as well as placing a new AtomicCounter in its slot and
	 * setting the element count to the proper value.
	 * @param p_element_count The new element count for the buffer.
	 * @return `OK` on success, and `ERR_OUT_OF_MEMORY` if failed, which should crash.
	 */
	Error _alloc_buffer(uint64_t p_size);

	/**
	 * @brief Reallocates the current buffer to a new size.
	 * @param p_element_count The new number of elements in the buffer.
	 * @return `OK` on success, and `ERR_OUT_OF_MEMORY` on failure, which should crash.
	 */
	Error _realloc_buffer(uint64_t p_size);

public:
	/**
	 * @brief Gets the given item from a given index into the vector. Assumes the user will modify the referenced data,
	 * so copies the vector to a new buffer.
	 * @param p_index The index into the vector we want to retrieve the data for
	 * @returns A reference to the item at the given index.
	 */
	FORCE_INLINE T &get(int64_t p_index) {
		ERR_OUT_OF_BOUNDS_FATAL(p_index, size());
		ERR_COND_FATAL(_copy_on_write() != OK);
		return _ptr[p_index];
	}

	/**
	 * @brief Gets the given item from a given index into the vector. Assumes the
	 * @param p_index The index into the vector we want to retrieve the data for
	 * @returns A constant reference to the item a the given index.
	 */
	FORCE_INLINE const T &get(int64_t p_index) const {
		ERR_OUT_OF_BOUNDS_FATAL(p_index, size());
		return _ptr[p_index];
	}

	/**
	 * @brief Sets the item at a given index to a specific value. Writing to the data means that the pointer is not
	 * equal across references, so a new array is allocated.
	 * @param p_index The given index to set it at
	 * @param item The given item to set
	 */
	FORCE_INLINE void set(const T &p_item, int64_t p_index) {
		ERR_OUT_OF_BOUNDS_FATAL(p_index, size());
		// Writing to the pointer modifies contents, create new buffer.
		ERR_COND_FATAL(_copy_on_write() != OK);
		_ptr[p_index] = p_item;
	}

	/**
	 * @brief Operator to assign a vector to another vector.
	 * @param p_from The other vector to copy into the current vector
	 */
	void operator=(const CoWData &p_from) {
		_ref(p_from);
	}

	/**
	 * @brief Moves the data from the given vector into this vector. Takes ownership of all relevant data.
	 * @param p_from The vector to move data from
	 */
	void operator=(CoWData<T> &&p_from) {
		if (_ptr == p_from._ptr) {
			return;
		}

		_unref();
		_ptr = p_from._ptr;
		p_from._ptr = nullptr;
	}

	/**
	 * @brief Gets the number of elements within the vector.
	 * @returns A value for the number of elements in the vector
	 */
	FORCE_INLINE int64_t size() const {
		return _ptr ? *_get_size() : 0;
	}

	/**
	 * @brief Method to find out if a vector type has any values in it.
	 * @returns `TRUE` if the vector is empty, `FALSE` if it is not.
	 */
	FORCE_INLINE bool is_empty() const {
		return _ptr == nullptr || *_get_size() == 0;
	}

	/**
	 * @brief Obtains the number of references to the given vector.
	 * @return The number of references to the given vector.
	 */
	FORCE_INLINE uint64_t get_reference_count() const {
		return _ptr ? _get_refc()->get() : 0;
	}

	/**
	 * @brief Gets the current pointer of the vector, with read and write permissions. Assumes a write is going to
	 * occur, so it copies all data over to a new buffer prior to being read. This can be expensive, so use `ptr()` for
	 * reading data without this issue.
	 * @returns The current pointer used to hold all of the items in the vector, with read/write permissions
	 */
	FORCE_INLINE T *ptrw() {
		ERR_COND_FATAL(_copy_on_write() != OK);
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
	 * @brief Resizes the vector to a new number of elements. Should not be relied on to properly resize the vector,
	 * and does initialize any new elements in the vector when called if their types are non-trivial.
	 * @param p_new_size The new number of elements in the vector
	 */
	Error resize(int64_t p_new_size);

	/**
	 * @brief Removes an item at a given index in the vector.
	 * @param index The index into the vector to remove from
	 * @returns `true` if the item was removed successfully, `false` if the index was invalid or the vector could not
	 * be resized
	 */
	void remove_at(int64_t p_index);

	/**
	 * @brief Inserts an item into the vector at a specific index. Takes ownership of the item in question
	 * @param item The item to move into the vector.
	 * @param index The index to insert the item at
	 * @returns `OK` on success, and an error code if something went wrong and the item was unable to be from inserted.
	 */
	Error insert(T &&p_item, int64_t p_index);

	/**
	 * @brief Appends an item to the end of the vector.
	 * @param item The item to add into the vector
	 */
	Error push_back(T &&p_item);

	/**
	 * @brief Clears all items out of the vector and set its size to zero. Functionally works like `_unref()` aside
	 * from freeing the pointer, as it assumes more values will be added within scope.
	 */
	FORCE_INLINE void clear() {
		_unref();
	}

	/**
	 * @brief Class constructor for a `CoWData` class. By default, it does nothing but obtain the stride value of the
	 * vector (the number of bytes between two datatypes), as this value may not be set properly at other points of
	 * initialization.
	 * @returns An instance of the `CoWData` class.
	 */
	FORCE_INLINE CoWData() {}
	FORCE_INLINE CoWData(const CoWData &p_from) {
		_ref(p_from);
	}

	FORCE_INLINE CoWData(CoWData &&p_from) {
		_ptr = p_from._ptr;
		p_from._ptr = nullptr;
	}

	/**
	 * @brief Class constructor for the `CoWData` type, which obtains a set of values from an `std::initializer_list`
	 * (such as `{1, 2, 3}`) and converts it into our own vector class.
	 * @param p_init The initializer list to construct the vector from
	 */
	FORCE_INLINE CoWData(std::initializer_list<T> p_init) {
		ERR_FAIL_COND(resize(p_init.size() * sizeof(T)) != OK);

		int i = 0;
		for (const T &element : p_init) {
			_ptr[i] = element;

			i++;
		}

		*_get_size() = i;
	}

	FORCE_INLINE ~CoWData() {
		_unref();
	}
};

template <typename T>
void CoWData<T>::_ref(const CoWData &p_from) {
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

template <typename T>
void CoWData<T>::_unref() {
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

template <typename T>
Error CoWData<T>::_copy_on_write() {
	if (!_ptr || _get_refc()->get() == 1) {
		return OK;
	}

	return _copy_to_new_buffer(size(), size());
}

template <typename T>
Error CoWData<T>::resize(int64_t p_new_size) {
	ERR_FAIL_COND_R(p_new_size < 0, ERR_INVALID_PARAMETER);

	int64_t current_size = size();

	if (current_size == p_new_size) {
		return OK; // no need for changes
	}

	if (current_size < p_new_size) {
		if (!_ptr) {
			Error err = _alloc_buffer(p_new_size);
			if (err) {
				return err;
			}
		} else if (_get_refc()->get() == 1) {
			Error err = _realloc_buffer(p_new_size);
			if (err) {
				return err;
			}
		} else {
			Error err = _copy_to_new_buffer(current_size, p_new_size);
			if (err) {
				return err;
			}
		}

		if constexpr (!std::is_trivially_constructible_v<T>) {
			for (int i = current_size; i < p_new_size; i++) {
				vnew_placement(_ptr + i, T());
			}
		}
	} else if (current_size > p_new_size) {
		// Destroy items that are no longer needed in the vector
		if (p_new_size == 0) {
			_unref();
			return OK;
		} else if (_get_refc()->get() == 1) {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				for (uint64_t i = p_new_size; i < *_get_size(); i++) {
					_ptr[i].~T();
				}
			}

			Error err = _realloc_buffer(p_new_size);
			if (err) {
				return err;
			}
		} else {
			return _copy_to_new_buffer(current_size, p_new_size);
		}
	}

	return OK;
}

template <typename T>
Error CoWData<T>::_copy_to_new_buffer(uint64_t p_old_count, uint64_t p_new_count) {
	const CoWData prev;
	prev._ptr = _ptr;
	_ptr = nullptr;

	Error err = _alloc_buffer(p_new_count);
	if (err) {
		_ptr = prev._ptr;
		prev._ptr = nullptr;
		return err;
	}

	memcpy_arr_placement(_ptr, prev._ptr, p_old_count > p_new_count ? p_new_count : p_old_count);
	return OK;
}

template <typename T>
Error CoWData<T>::_alloc_buffer(uint64_t p_element_count) {
	T *ptr = (T *)Memory::vallocate((p_element_count * sizeof(T)) + DATA_OFFSET);
	ERR_COND_NULL_R(ptr, ERR_OUT_OF_MEMORY);

	_ptr = (T *)(((uint8_t *)ptr) + DATA_OFFSET);

	new (_get_refc()) AtomicCounter<uint64_t>(1);
	*_get_size() = p_element_count;
	return OK;
}

template <typename T>
Error CoWData<T>::_realloc_buffer(uint64_t p_element_count) {
	T *nptr = (T *)Memory::vreallocate(((uint8_t *)_ptr) - DATA_OFFSET, (p_element_count * sizeof(T)) + DATA_OFFSET);
	ERR_COND_NULL_R(nptr, ERR_OUT_OF_MEMORY);

	_ptr = (T *)(((uint8_t *)nptr) + DATA_OFFSET);
	ERR_FAIL_COND_R(_get_refc()->get() != 1, ERR_BUG);
	*_get_size() = p_element_count;
	return OK;
}

template <typename T>
void CoWData<T>::remove_at(int64_t p_index) {
	// Check if the index is out of bounds or not
	ERR_OUT_OF_BOUNDS(p_index, size());

	uint64_t old_size = size();

	if (old_size == 1) {
		_unref();
		return;
	}

	uint64_t new_size = old_size - 1;

	if (_get_refc()->get() == 1) {
		_ptr[p_index].~T();
		// Copy data down to the current pointer position.
		Memory::vmemmove(_ptr + p_index, _ptr + p_index + 1, (new_size - p_index) * sizeof(T));
		// Delete content at the end of the buffer
		Error err = _realloc_buffer(new_size);
		ERR_COND_FATAL(err != OK);
	} else {
		// Copy data to a new buffer
		T *other = _ptr;
		_ptr = (T *)Memory::vallocate(new_size);
		if (std::is_trivially_copyable_v<T>) {
			Memory::vcopy_memory(_ptr, other, p_index * sizeof(T));
			Memory::vcopy_memory(_ptr + p_index, other + p_index, (new_size - p_index) * sizeof(T));
		} else {
			for (int i = 0; i < p_index; i++) {
				vnew_placement(_ptr + i, T(other[i]));
			}

			for (uint64_t i = p_index; i < new_size; i++) {
				vnew_placement(_ptr + i, T(other[i]));
			}
		}
	}
}

template <typename T>
Error CoWData<T>::insert(T &&p_item, int64_t p_index) {
	const int64_t new_size = size() + 1;
	ERR_OUT_OF_BOUNDS_R(p_index, new_size, ERR_INVALID_PARAMETER);

	const int64_t old_size = size();

	if (!_ptr) {
		Error err = _alloc_buffer(new_size);
		if (err) {
			return err;
		}
	} else {
		if (_get_refc()->get() == 1) {
			Error err = _realloc_buffer(new_size);
			if (err) {
				return err;
			}

		} else {
			Error err = _copy_to_new_buffer(old_size, new_size);
			if (err) {
				return err;
			}
		}
		Memory::vmemmove(_ptr + p_index + 1, _ptr + p_index, (old_size - p_index) * sizeof(T));
	}

	vnew_placement(_ptr + p_index, T(std::move(p_item)));
	return OK;
}

template <typename T>
Error CoWData<T>::push_back(T &&p_item) {
	const uint64_t new_size = size() + 1;

	if (!_ptr) {
		_alloc_buffer(1);
	} else if (_get_refc()->get() == 1) {
		Error err = _realloc_buffer(new_size);
		if (err != OK) {
			return err;
		}
	} else {
		Error err = _copy_to_new_buffer(size(), new_size);
		if (err != OK) {
			return err;
		}
	}

	vnew_placement(_ptr + new_size - 1, T(std::move(p_item)));
	return OK;
}
