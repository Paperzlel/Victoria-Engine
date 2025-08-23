#pragma once

#include "core/typedefs.h"

class Variant;
class ArrayData;

class Array {
private:
    mutable ArrayData *_data;

    void _ref(const Array &p_from) const;
    void _unref() const;
public:
    
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

    Variant &operator[](u64 p_index);
    const Variant &operator[](u64 p_index) const;

    void operator=(const Array &p_other);

    bool operator==(const Array &p_other);
    bool operator!=(const Array &p_other);

    u64 size() const;
    bool is_empty() const;
    bool is_read_only() const;
    void set_read_only(bool p_bool) const;

    void append(const Variant &p_item);
    void remove_at(u64 p_index);
    void insert(u64 p_index, const Variant &p_item);

    Variant pop_front();
    Variant pop_back();
    void push_front(const Variant &p_item);
    void push_back(const Variant &p_item);

    void clear();
    void fill(const Variant &p_item);

    bool is_equal(const Array &p_other) const;

    Array(const Array &p_from);
    Array();

    ~Array();
};
