#pragma once

#include "core/typedefs.h"
#include "array.h"
#include "core/string/vstring.h"

typedef Vector<u8> ByteArray;
typedef Vector<i32> Int32Array;
typedef Vector<i64> Int64Array;
typedef Vector<float> Float32Array;
typedef Vector<double> Float64Array;

class Variant {
public:
    enum Type {
        // Atomic types
        NIL,
        BOOL,
        INT,
        FLOAT,
        STRING,

        // Non-typed arrays
        ARRAY,

        // Typed arrays
        BYTE_ARRAY,
        INT32_ARRAY,
        INT64_ARRAY,
        FLOAT32_ARRAY,
        FLOAT64_ARRAY,

        VARIANT_MAX
    };
private:

    Type type = NIL;

    struct ArrayRefBase {
        Refcount ref_count;
        FORCE_INLINE ArrayRefBase *reference() {
            if (ref_count.ref()) {
                return this;
            } else {
                return nullptr;
            }
        }

        static FORCE_INLINE ArrayRefBase *reference_from(ArrayRefBase *p_arr, ArrayRefBase *p_from) {
            if (p_arr == p_from) {
                return p_arr;
            }

            if (p_from->reference()) {
                if (p_arr->ref_count.unref()) {
                    delete p_arr;
                }
                return p_from;
            } else {
                return p_arr;
            }
        }

        static FORCE_INLINE void destroy(ArrayRefBase *p_arr) {
            if (p_arr->ref_count.unref()) {
                delete p_arr;
            }
        }
        FORCE_INLINE virtual ~ArrayRefBase() {}
    };

    template <typename T>
    struct ArrayRef : public ArrayRefBase {
        Vector<T> array;
        static FORCE_INLINE ArrayRef<T> *create() {
            return new ArrayRef<T>;
        }

        static FORCE_INLINE ArrayRef<T> *create(const Vector<T> &p_from) {
            return new ArrayRef<T>(p_from);
        }

        static FORCE_INLINE Vector<T> &get_array(ArrayRefBase *p_from) {
            return static_cast<const ArrayRef<T> *>(p_from)->array;
        }

        FORCE_INLINE ArrayRef() {
            ref_count.set(1);
        }

        FORCE_INLINE ArrayRef(const Vector<T> &p_from) {
            array = p_from;
            ref_count.set(1);
        }
    };

    union {
        bool _bool;
        i64 _int;
        double _float;
        ArrayRefBase *_array;
        void *_ptr; // General pointer type
        u8 _mem[sizeof(double) * 4] { 0 };
    } _data alignas(8);

    FORCE_INLINE void clear() {
        bool needs_freeing[VARIANT_MAX] = {
            false,  // NIL
            false,  // BOOL
            false,  // INT
            false,  // FLOAT
            true,   // STRING

            true,   // ARRAY

            true,   // BYTE_ARRAY
            true,   // INT32_ARRAY
            true,   // INT64_ARRAY
            true,   // FLOAT32_ARRAY
            true,   // FLOAT64_ARRAY
        };

        if (needs_freeing[type]) {
            _clear_internals();
        }
        type = NIL;
    }

    void _clear_internals();
public:

    String stringify(int recursion_count = 0) const;

    static Type get_return_type(const Variant &p_var) { return p_var.type; }

    void _ref(const Variant &p_other);
    void operator=(const Variant &p_var);

    bool operator==(const Variant &other);
    bool operator!=(const Variant &other);

    bool hash_compare(const Variant &p_other, int recursion_count) const;

    operator bool() const;
    operator i8() const;
    operator i16() const;
    operator i32() const;
    operator i64() const;
    operator u8() const;
    operator u16() const;
    operator u32() const;
    operator u64() const;
    operator float() const;
    operator double() const;
    operator String() const;
    operator Array() const;
    operator ByteArray() const;
    operator Int32Array() const;
    operator Int64Array() const;
    operator Float32Array() const;
    operator Float64Array() const;


    Variant(i8 p_int);
    Variant(i16 p_int);
    Variant(i32 p_int);
    Variant(i64 p_int);
    Variant(u8 p_uint);
    Variant(u16 p_uint);
    Variant(u32 p_uint);
    Variant(u64 p_uint);
    Variant(bool p_bool);
    Variant(float p_float);
    Variant(double p_double);
    Variant(const String &p_string);
    Variant(const char *const p_cstring);
    Variant(const Array &p_array);
    Variant(const ByteArray &p_byte_array);
    Variant(const Int32Array &p_int32_array);
    Variant(const Int64Array &p_int64_array);
    Variant(const Float32Array &p_float32_array);
    Variant(const Float64Array &p_float64_array);

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
