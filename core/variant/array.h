#pragma once

#include "core/typedefs.h"

class Variant;
class ArrayData;

class Array {
private:
    mutable ArrayData *_data;

    /**
     * @brief Creates data from the referenced array. Unreferences data if there is a reference and copies the internal data over otherwise.
     * @param p_from The array to reference from.
     */
    void _ref(const Array &p_from) const;
    /**
     * @brief Unreferences the current data. Decrements the internal refcount and deletes it if needed.
     */
    void _unref() const;
public:
    
    // Iterator struct. Used by C++ natively in `for i in arr` lists.
    struct Iterator {
        FORCE_INLINE Variant &operator*() const;
        FORCE_INLINE Variant *operator->() const;

        FORCE_INLINE Iterator &operator++();
        FORCE_INLINE Iterator &operator--();

        FORCE_INLINE bool operator==(const Iterator &p_other) const { return _p == p_other._p; }
        FORCE_INLINE bool operator!=(const Iterator &p_other) const { return _p != p_other._p; }

        FORCE_INLINE Iterator(Variant *p_element_ptr) { _p = p_element_ptr; }
        FORCE_INLINE Iterator() {}
        FORCE_INLINE Iterator(const Iterator &p_other) {_p = p_other._p; }
    private:
        Variant *_p = nullptr;
    };

    Iterator begin();
    Iterator end();

    /**
     * @brief Index operator (non-const)
     * @param p_index The index to obtain data from
     * @return The item at the given index
     */
    Variant &operator[](u64 p_index);
    /**
     * @brief Index operator (const)
     * @param p_index The index to obtain data from
     * @return The item at the given index
     */
    const Variant &operator[](u64 p_index) const;

    /**
     * @brief Assignment operator
     * @param p_other The array to assign from
     */
    void operator=(const Array &p_other);

    /**
     * @brief Equivalence operator
     * @param p_other The array to compare with
     * @return `true` if the same, `false` if not.
     */
    bool operator==(const Array &p_other);
    /**
     * @brief Non-equivalence operator
     * @param p_other The array to compare with
     * @return `false` if the same, `true` if not.
     */
    bool operator!=(const Array &p_other);

    /**
     * @brief Obtains the number of elements inside the array.
     * @return The element count of the array
     */
    u64 size() const;
    /**
     * @brief Checks to see if the array is empty or not.
     * @return `true` if it is empty, `false` if not.
     */
    bool is_empty() const;
    /**
     * @brief Checks to see if the array is read-only or not.
     * @return `true` if read-only, `false` if not.
     */
    bool is_read_only() const;
    /**
     * @brief Sets the array to read-only depending on the value.
     * @param p_bool The value to set it to.
     */
    void set_read_only(bool p_bool) const;

    /**
     * @brief Appends data to the end of the array.
     * @param p_item Reference to the item to append
     */
    void append(const Variant &p_item);
    /**
     * @brief Removes the item at the given index from the array.
     * @param p_index The index to remove at.
     */
    void remove_at(u64 p_index);
    /**
     * @brief Inserts an item into the array at a given index. Index should be between 0 and the size of the array.
     * @param p_index The index to insert at.
     * @param p_item The item to insert
     */
    void insert(u64 p_index, const Variant &p_item);

    /**
     * @brief Obtains the item at the front of the array and removes it from the array.
     * @return A copy of the data from the front of the array
     */
    Variant pop_front();
    /**
     * @brief Obtains the item at the back of the array and removes it.
     * @return A copy of the data from the back of the array
     */
    Variant pop_back();
    /**
     * @brief Pushes data to the front of the array
     * @param p_item The item to add to the front.
     */
    void push_front(const Variant &p_item);
    /**
     * @brief Pushes data to the back of the array
     * @param p_item The item to append to the front
     */
    void push_back(const Variant &p_item);

    /**
     * @brief Clears all data from the array
     */
    void clear();
    /**
     * @brief Fills the array with a specific item. The array is not resized for this operation.
     * @param p_item The item to fill the array with
     */
    void fill(const Variant &p_item);

    /**
     * @brief Checks to see if the two array are equal. Same as the equivalence operator.
     * @param p_other The array to compare with.
     * @return `true` if equal, `false` if not
     */
    bool is_equal(const Array &p_other) const;

    /**
     * @brief Array class constructor (from other array)
     */
    Array(const Array &p_from);
    /**
     * @brief Array class constructor
     */
    Array();

    /**
     * @brief Array class destructor
     */
    ~Array();
};
