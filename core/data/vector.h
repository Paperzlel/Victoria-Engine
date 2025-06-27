#pragma once

#include "core/typedefs.h"
#include "core/error/error_macros.h"
#include "core/os/static_allocator.h"
#include "atomic_counter.h"

#include <list>

/**
 * @brief Our core vector class. Acts as an implementation of `std::vector`, which we use over the latter to keep our code lightweight and readable for end-users. This class consists of one pointer which acts as the raw vector to access and retrieve values from, and several `uint64`s that keep count of its size, elements and the standalone data size. For situations where one may alternate between datatypes, please refer to the `Array` class, which can be found under `core/variant/array.h`. 
 */
template <typename T>
class Vector {
private:
    friend class String;

    Refcount refc;
    // The size of the array in bytes
    uint64 p_size = 0;
    // The number of elements in the array
    uint64 p_element_count = 0;
    // The internal pointer that makes up the actual array in memory
    T *_ptr = nullptr;

    FORCE_INLINE void _ref(const Vector &p_from);
    FORCE_INLINE void _unref();
    FORCE_INLINE void _copy_on_write();
    FORCE_INLINE Error _resize(uint64 n_size);
    FORCE_INLINE Error _realloc(uint64 n_size);
public:

    /**
     * @brief Gets the given item from a given index into the vector.
     * @param p_index The index into the vector we want to retrieve from
     * @returns An item of type `T`
     */
    FORCE_INLINE T &get(uint64 p_index) const {
        ERR_OUT_OF_BOUNDS_FATAL(p_index, p_element_count);
        return _ptr[p_index];
    }

    /**
     * @brief Sets the item at a given index to a specific value.
     * @param p_index The given index to set it at
     * @param item The given item to set
     */
    FORCE_INLINE void set(T &item, uint64 p_index) {
        ERR_OUT_OF_BOUNDS_FATAL(p_index, p_element_count);
        // Since the data is set up so that data isn't unique - not normally a problem for basic (atomic) data, but for Strings (data types with a nested vector) it has a whole load of problems. To fix this, we need to copy over data properly, into the block itself
        _copy_on_write();
        _ptr[p_index] = item;
    }

    // Iterator structure, is used by a C++ for loop in order to find an element at a given point in the vector. Its operators are used to find the adjacent values in memory.
    struct Iterator {
        FORCE_INLINE T &operator*() const { return *element_ptr; }

        FORCE_INLINE T *operator->() const { return element_ptr; }
        FORCE_INLINE Iterator &operator++() {
            element_ptr++;
            return *this;
        }
        FORCE_INLINE Iterator &operator--() {
            element_ptr--;
            return *this;
        }

        FORCE_INLINE bool operator==(const Iterator &other) const { return element_ptr == other.element_ptr; }
        FORCE_INLINE bool operator!=(const Iterator &other) const { return element_ptr != other.element_ptr; }

        Iterator(T *p_ptr) { element_ptr = p_ptr; }
        Iterator() {}
        Iterator(const Iterator &p_iter) { element_ptr = p_iter.element_ptr; }

    private: 
        T *element_ptr = nullptr;
    };

    // Constructs an `Iterator` from the first element in the vector.
    FORCE_INLINE Iterator begin() {
        return Iterator(_ptr);
    }

    // Constructs an `Iterator` from the last element in the vector.
    FORCE_INLINE Iterator end() {
        return Iterator(_ptr + p_element_count);
    }

    /**
     * @brief Operator to access an item at the given index.
     * @param index The index in the vector to access
     * @returns The item at the given index
     */
    FORCE_INLINE T &operator[](int index) { 
        // Check for an out of bounds index and throw an error if so
        ERR_OUT_OF_BOUNDS_FATAL(index, p_element_count - 1);

        return ((Vector<T> *)(this))->_ptr[index];
    }
    /**
     * @brief Operator to access the item at the given index.
     * @param index The index in the vector to access
     * @returns The item at the given index
     */
    FORCE_INLINE const T &operator[](int index) const { 
        ERR_OUT_OF_BOUNDS_FATAL(index, p_element_count - 1);

        return ((Vector<T> *)(this))->_ptr[index];
    }

    /**
     * @brief Operator to assign a vector to another vector.
     * @param p_from The other vector to copy into the current vector
     */
    inline void operator=(const Vector &p_from) {
        _ref(p_from);
    }

    FORCE_INLINE bool operator==(const Vector &p_other) const;

    FORCE_INLINE bool operator!=(const Vector &p_other) const;

    /**
     * @brief Gets the number of elements within the vector.
     * @returns A value for the number of elements in the vector
     */
    FORCE_INLINE uint64 size() const { return p_element_count; };
    /**
     * @brief Method to find out if a vector type has any values in it.
     * @returns `TRUE` if the vector is empty, `FALSE` if it is not.
     */
    FORCE_INLINE bool is_empty() const { return p_element_count == 0; };
    /**
     * @brief Gets the number of bytes the vector currently has allocated to itself. 
     * @returns The size of the vector in bytes
     */
    FORCE_INLINE uint64 get_ptr_size() const { return p_size; };

    // NOTE: probably unsafe for now
    
    /**
     * @brief Gets the current pointer of the vector, with read and write permissions. It is strongly advised to not access this directly unless absolutely necessary, as any modification to the pointer will not update any other information it holds.
     * @returns The current pointer used to hold all of the items in the vector, with read/write permissions
     */
    FORCE_INLINE T *ptrw() { return _ptr; }
    /**
     * @brief Gets the current pointer of the vector, as a read-only pointer. If you need to get the direct value held by a vector (i.e. if it is `nullptr` or not) then this method should be used over `ptrw()`.
     * @returns The current pointer used to hold all of the items in the vector, with readonly permissions
     */
    FORCE_INLINE const T *ptr() const { return _ptr; }

    /**
     * @brief Obtains the current refcount object.
     */
    FORCE_INLINE Refcount &get_refc() const { return ((Vector<T> *)(this))->refc; }

    /**
     * @brief Resizes the vector to a new number of elements. Differs from `_resize` in that it automatically calculates the number of 
     * bytes per-element.
     * @param p_new_size The new number of elements in the vector
     */
    FORCE_INLINE Error resize(int p_new_size) { return _resize(p_new_size * sizeof(T)); }

    FORCE_INLINE void append(T item);
    FORCE_INLINE void remove_at(uint64 index);
    FORCE_INLINE void insert_at(T item, uint64 index);

    FORCE_INLINE void push_front(T item);
    FORCE_INLINE T pop_front();
    FORCE_INLINE void push_back(T item);
    FORCE_INLINE T pop_back();
    
    FORCE_INLINE void clear();

    FORCE_INLINE int find(T item) const;

    /**
     * @brief Class constructor for a `Vector` class. By default, it does nothing but obtain the stride value of the vector (the number of bytes between two datatypes), as this value may not be set properly at other points of initialization.
     * @returns An instance of the `Vector` class.
     */
    FORCE_INLINE Vector() {}
    FORCE_INLINE Vector(const Vector &p_from);

    /**
     * @brief Class constructor for the `Vector` type, which obtains a set of values from an `std::initializer_list` (such as `{1, 2, 3}`) and converts it into our own vector class.
     * @param p_init The initializer list to construct the vector from
     */
    FORCE_INLINE Vector(std::initializer_list<T> p_init) {
        ERR_FAIL_COND(_resize(p_init.size() * data_size) != OK);

        p_element_count = 0;
        for (const T &element : p_init) {
            _ptr[p_element_count] = element;

            p_element_count++;
        }
        this->p_element_count = p_element_count;
    }

    FORCE_INLINE ~Vector();
};

/**
 * @brief Private initializer method. It's used instead of simply copying memory over, as there are several steps one may need to go through in order for the data to be copied properly, namely any datatypes than cannot be trivially copied over.
 * @param p_from The other vector to copy data from
 */
template<typename T>
void Vector<T>::_ref(const Vector &p_from) {
    if (_ptr == p_from._ptr) {
        return;
    }

    _unref();
    _ptr = nullptr;
    p_size = 0;
    p_element_count = 0;

    if (!p_from._ptr) {
        return;
    }

    Error err = _resize(p_from.p_size);
    ERR_FAIL_COND(err);

    if (p_from.get_refc().get() > 0) {
        p_from.get_refc().ref();
    }
    // NOTE: This seems like a hack, but it's done for good reason. Without copying the memory into the location - just assigning it normally - we would get a free_base error when running, as the memory would be referenced in two places and still in use - not good. As a result, we copy it in - letting the STL know that it's being used, and that it should be freed when it is ready to be killed.
    if (!std::is_trivially_copyable<T>::value) {
        for (int i = 0; i < p_element_count; i++) {
            new (&_ptr[i]) T(p_from._ptr[i]);
        }
    } else {
        StaticAllocator::vcopy_memory(_ptr, p_from._ptr, p_from.p_size);
    }
}

/**
 * @brief Frees all of the data currently held within the vector, and sets the pointer to null. Since not all data is easy to get rid of (and can have some very bad side effects), we use this method instead as it prevents any issue with non-trivial types being corrupted in any way.
 */
template<typename T>
void Vector<T>::_unref() {
    if (!_ptr) {
        return;
    }

    if (refc.unrefval() > 0) {
        _ptr = nullptr;
        return;
    }

    if (!std::is_trivially_destructible<T>::value) {
        uint64 current_size = size();

        for (int i = 0; i < current_size; i++) {
            T *t = &_ptr[i];
            t->~T();
        }
    }

    StaticAllocator::vfree(_ptr, p_size);
}

template<typename T>
void Vector<T>::_copy_on_write() {
    if (refc.get() > 1) {
        T *new_data = (T *)StaticAllocator::vallocate(p_size);
        ERR_COND_FATAL(new_data);

        if (!std::is_trivially_copyable<T>::value) {
            for (int i = 0; i < p_element_count; i++) {
                new (&new_data[i]) T(_ptr[i]);
            }
        } else {
            StaticAllocator::vcopy_memory(new_data, _ptr, p_size);
        }

        _unref();
        _ptr = new_data;
        refc.set(1);
    }
}

/**
 * @brief Changes the size of the current vector to a new given size. Sets `p_size` and `p_element_count` itself in order to prevent any potential errors otherwise.
 * @param n_size The new size of the vector in bytes
 * @return `OK` if the vector was able to be resized, and returns a given error message if something went wrong. 
*/
template <typename T>
Error Vector<T>::_resize(uint64 n_size) {
    ERR_FAIL_COND_R(n_size < 0, ERR_INVALID_PARAMETER);

    uint64 current_size = get_ptr_size();
    uint64 n_element_count = n_size / sizeof(T);

    if (current_size == n_size) {
        return OK; // no need for changes
    }

    if (n_size == 0) {
        _unref();
        _ptr = nullptr;
        p_size = 0;
        p_element_count = 0;
        return OK;
    }

    // copy data to a new alloc beforehand
    _copy_on_write();

    if (current_size < n_size) {
        // Allocate if the current size is 0
        if (current_size == 0) {
            T *n_data = (T *)StaticAllocator::vallocate(n_size);
            ERR_COND_NULL_R(n_data, ERR_OUT_OF_MEMORY);
            _ptr = n_data;
            refc.set(1);
        } else {
            Error err = _realloc(n_size);
            if (err) {
                return err;
            }
        }


        // Check if the classes can't be constructed easily, if not call constructors (because this happens before raw assignment, it prevents data being wiped)
        if (!std::is_trivially_constructible<T>::value) {
            for (int i = (current_size / sizeof(T)); i < n_element_count; i++) {
                new (&_ptr[i]) T;
            }
        } else {
            // Zero out the memory so that garbage data isn't read
            StaticAllocator::vzero(_ptr + p_element_count, n_size - p_size);
        }
        p_size = n_size;

    } else if (current_size > n_size) {
        // Destroy items that are no longer needed in the vector
        if (!std::is_trivially_destructible<T>::value) {
            for (uint64 i = n_size; i < p_size; i++) {
                T *t = &_ptr[i];
                t->~T();
                delete &_ptr[i]; // Call delete here rather than in `_unref()`, because we no longer need these specific values, but still require the array as a whole.
            }
        }

        Error err = _realloc(n_size);
        if (err) {
            return err;
        }
        p_size = n_size;
    }
    p_element_count = n_element_count;

    return OK;
}

template<typename T>
Error Vector<T>::_realloc(uint64 n_size) {
    T *n_data = (T *)StaticAllocator::vreallocate(_ptr, p_size, n_size);
    ERR_COND_NULL_R(n_data, ERR_OUT_OF_MEMORY);
    
    refc.set(1); // Reset refcount due to unique size
    _ptr = n_data;
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
 * @brief Removes an item at a given index in the vector.
 * @param index The index into the vector to remove from
 * @returns `true` if the item was removed successfully, `false` if the index was invalid or the vector could not be resized
 */
template <typename T>
void Vector<T>::remove_at(uint64 index) {
    // Check if the index is out of bounds or not
    ERR_OUT_OF_BOUNDS(index, p_element_count - 1);

    //Get a list of every other index less than the given index
    uint64 less_than = index;

    Vector<T> less_arr;
    for (int i = 0; i < less_than; i++) {
        less_arr.append(this->operator[](i));
    }

    Vector<T> greater_arr;
    for (int i = index + 1; i < p_element_count; i++) {
        greater_arr.append(this->operator[](i));
    }

    uint64 new_size = (p_element_count - 1) * sizeof(T);

    ERR_FAIL_COND(_resize(new_size) != OK);

    for (int i = 0; i <= p_element_count - 1; i++) {
        if (p_element_count == 1) {
            // Last element in the array
            clear();
            break;
        }
        if (less_arr.size() == 0) {
            _ptr[i] = greater_arr[i];
            continue;
        }
        if (greater_arr.size() == 0) {
            _ptr[i] = less_arr[i];
            continue;
        }

        if (i <= less_than - 1) {
            _ptr[i] = less_arr[i];
        } else {
            _ptr[i] = greater_arr[i - less_than];
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
void Vector<T>::insert_at(T item, uint64 index) {
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
    set(item, size() - 1);
}

/**
 * @brief Removes and gets the item at the end of the vector.
 * @returns The item at the end of the vector
 */
template <typename T>
T Vector<T>::pop_back() {
    T item = get(p_element_count - 1);
    remove_at(p_element_count - 1);
    return item;
}

/**
 * @brief Clears all items out of the vector and set its size to zero. Functionally works like `_unref()` aside from freeing the pointer, as it assumes more values will be added within scope.
 */
template <typename T>
void Vector<T>::clear() {
    _resize(0);
    p_element_count = 0;
    p_size = 0;
}

/**
 * @brief Looks for whether a given item exists within the vector, and gets its respective position. If the item cannot be found, it returns `-1`, which should be treated like returning null.
 * @param item The item to look for within the vector.
 * @returns The index of the given item if it was found, and `-1` if it fails to find the item.
 */
template <typename T>
int Vector<T>::find(T item) const {
    for (int i = 0; i < size(); i++) {
        if (_ptr[i] == item) {
            return i;
        }
    }
    
    return -1;
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
