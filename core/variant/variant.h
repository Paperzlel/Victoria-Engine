#pragma once

#include "core/typedefs.h"
#include "array.h"

class Variant {
public:
    enum Type {
        NIL,
        BOOL,
        INT,
        FLOAT,

        ARRAY,
        VARIANT_MAX
    };
private:

    Type type = NIL;

    union {
        bool _bool;
        int64 _int;
        double _float;
        void *_ptr; // General pointer type
        uint8 _mem[sizeof(float) * 4] { 0 };
    } _data alignas(8);

    FORCE_INLINE void clear() {
        bool needs_freeing[VARIANT_MAX] = {
            false,
            false,
            false,
            false,
            true,

        };

        if (needs_freeing[type]) {
            _clear_internals();
        }
        type = NIL;
    }

    void _clear_internals();
public:

    const char *get_value_as_string();

    static Type get_return_type(const Variant &p_var) { return p_var.type; }

    void operator=(const Variant &p_var);

    bool operator==(const Variant &other);
    bool operator!=(const Variant &other);

    bool hash_compare(const Variant &p_other, int recursion_count) const;

    operator bool() const;
    operator int8() const;
    operator int16() const;
    operator int32() const;
    operator int64() const;
    operator uint64() const;
    operator uint32() const;
    operator uint16() const;
    operator uint8() const;
    operator float() const;
    operator double() const;


    Variant(int8 p_int);
    Variant(int16 p_int);
    Variant(int32 p_int);
    Variant(int64 p_int);
    Variant(uint8 p_uint);
    Variant(uint16 p_uint);
    Variant(uint32 p_uint);
    Variant(uint64 p_uint);
    Variant(bool p_bool);
    Variant(float p_float);
    Variant(double p_double);
    Variant(const Array &p_array);

    Variant() { type = NIL; }
    ~Variant() {
        clear();
    }
};

Variant &Array::Iterator::operator*() const {
    return *_p;
}

Variant *Array::Iterator::operator->() const {
    return _p;
}

Array::Iterator &Array::Iterator::operator++() {
    _p++;
    return *this;
}

Array::Iterator &Array::Iterator::operator--() {
    _p--;
    return *this;
}
