#include "variant.h"

void Variant::_clear_internals() {
    switch (type) {
        case STRING: {
            reinterpret_cast<String *>(_data._mem)->~String();
        } break;
        case ARRAY: {
            reinterpret_cast<Array *>(_data._mem)->~Array();
        } break;
        case BYTE_ARRAY: {
            ArrayRefBase::destroy(_data._array);
        } break;
        case INT32_ARRAY: {
            ArrayRefBase::destroy(_data._array);
        } break;
        case INT64_ARRAY: {
            ArrayRefBase::destroy(_data._array);
        } break;
        case FLOAT32_ARRAY: {
            ArrayRefBase::destroy(_data._array);
        } break;
        case FLOAT64_ARRAY: {
            ArrayRefBase::destroy(_data._array);
        } break;
        default: {

        } break;
    }
}

String stringify_clean(const Variant &p_var, int recursion_count) {
    String s = p_var.stringify(recursion_count);
    return s;
}

template<typename T>
String stringify_vector(const T &p_vec, int recursion_count) {
    String ret("[");
    for (int i = 0; i < p_vec.size(); i++) {
        if (i > 0) {
            ret += ", ";
        }

        ret += stringify_clean(p_vec[i], recursion_count);
    }

    ret += "]";
    return ret;
}

String Variant::stringify(int recursion_count) const  {
    switch (type) {
        case NIL: 
            return "<null>";
        case BOOL: 
            return _data._bool ? "true" : "false";
        case INT:
        case FLOAT: 
            return ""; //TODO: handle
        case STRING:
            return *reinterpret_cast<const String *>(_data._mem);
        case ARRAY: {
            ERR_FAIL_COND_MSG_R(recursion_count > 1024, "Do not self-assign arrays.", String());

            recursion_count++;
            return stringify_vector(operator Array(), recursion_count);
        }
        case BYTE_ARRAY:
            return stringify_vector(operator ByteArray(), recursion_count);
        case INT32_ARRAY:
            return stringify_vector(operator Int32Array(), recursion_count);
        case INT64_ARRAY:
            return stringify_vector(operator Int64Array(), recursion_count);
        case FLOAT32_ARRAY:
            return stringify_vector(operator Float32Array(), recursion_count);
        case FLOAT64_ARRAY:
            return stringify_vector(operator Float64Array(), recursion_count);
        default: 
            return "";
    }
}

void Variant::_ref(const Variant &p_other) {
    clear();

    type = p_other.type;

    switch(type) {
        case NIL: {

        } break;
        case BOOL: {
            _data._bool = p_other._data._bool;
        } break;
        case INT: {
            _data._int = p_other._data._int;
        } break;
        case FLOAT: {
            _data._float = p_other._data._float;
        } break;
        case STRING: {
            new (_data._mem) String(*reinterpret_cast<const String *>(p_other._data._mem));
        } break;
        case ARRAY: {
            new (_data._mem) Array(*reinterpret_cast<const Array *>(p_other._data._mem));
        } break;
        case BYTE_ARRAY: {
            _data._array = static_cast<ArrayRef<u8> *>(p_other._data._array)->reference();
            if (!_data._array) {
                _data._array = ArrayRef<u8>::create();
            }
        } break;
        case INT32_ARRAY: {
            _data._array = static_cast<ArrayRef<i32> *>(p_other._data._array)->reference();
            if (!_data._array) {
                _data._array = ArrayRef<i32>::create();
            }
        } break;
        case INT64_ARRAY: {
            _data._array = static_cast<ArrayRef<i64> *>(p_other._data._array)->reference();
            if (!_data._array) {
                _data._array = ArrayRef<i64>::create();
            }
        } break;
        case FLOAT32_ARRAY: {
            _data._array = static_cast<ArrayRef<float> *>(p_other._data._array)->reference();
            if (!_data._array) {
                _data._array = ArrayRef<float>::create();
            }
        } break;
        case FLOAT64_ARRAY: {
            _data._array = static_cast<ArrayRef<double> *>(p_other._data._array)->reference();
            if (!_data._array) {
                _data._array = ArrayRef<double>::create();
            }
        }
        default: {
        }
    }
}

void Variant::operator=(const Variant &p_var) {

    if (type != p_var.type) {
        _ref(p_var);
        return;
    }

    switch(p_var.type) {
        case NIL: {
            _data._ptr = nullptr;
        } break;
        case BOOL: {
            _data._bool = p_var._data._bool;
        } break;
        case INT: {
            _data._int = p_var._data._int;
        } break;
        case FLOAT: {
            _data._float = p_var._data._float;
        } break;
        case STRING: {
            *reinterpret_cast<String *>(_data._mem) = *reinterpret_cast<const String *>(p_var._data._mem);
        } break;
        case ARRAY: {
            *reinterpret_cast<Array *>(_data._mem) = *reinterpret_cast<const Array *>(p_var._data._mem);
        } break;
        case BYTE_ARRAY: {
            _data._array = static_cast<ArrayRef<u8> *>(p_var._data._array)->reference();
        } break;
        case INT32_ARRAY: {
            _data._array = static_cast<ArrayRef<i32> *>(p_var._data._array)->reference();
        } break;
        case INT64_ARRAY: {
            _data._array = static_cast<ArrayRef<i64> *>(p_var._data._array)->reference();
        } break;
        case FLOAT32_ARRAY: {
            _data._array = static_cast<ArrayRef<float> *>(p_var._data._array)->reference();
        } break;
        case FLOAT64_ARRAY: {
            _data._array = static_cast<ArrayRef<double> *>(p_var._data._array)->reference();
        }
        default:
            break;
    } 
}

bool Variant::operator==(const Variant &other) {
    return hash_compare(other, 0);
}

bool Variant::operator!=(const Variant &other) {
    return !hash_compare(other, 0);
}


bool Variant::hash_compare(const Variant &p_other, int recursion_count) const {
    if (type != p_other.type) {
        return false;
    }

    switch (type) {
        case BOOL: {
            return _data._bool == p_other._data._bool;
        } break;
        case INT: {
            return _data._int == p_other._data._int;
        } break;
        case FLOAT: {
            return _data._float == p_other._data._float;
        } break;
        case STRING: {
            const String &l = *(reinterpret_cast<const String *>(_data._mem));
            const String &r = *(reinterpret_cast<const String *>(p_other._data._mem));
            if (l != r) {
                return false;
            }

            return true;
        } break;
        case ARRAY: {
            const Array &l = *(reinterpret_cast<const Array *>(_data._mem));
            const Array &r = *(reinterpret_cast<const Array *>(p_other._data._mem));
            if (!l.is_equal(r)) {
                return false;
            }

            return true;
        } break;

        case BYTE_ARRAY: {
            const ArrayRef<u8> *l = (reinterpret_cast<const ArrayRef<u8> *>(_data._array));
            const ArrayRef<u8> *r = (reinterpret_cast<const ArrayRef<u8> *>(p_other._data._array));

            if (l->array != r->array) {
                return false;
            }

            return true;
        } break;
        case INT32_ARRAY: {
            const ArrayRef<i32> *l = (reinterpret_cast<const ArrayRef<i32> *>(_data._array));
            const ArrayRef<i32> *r = (reinterpret_cast<const ArrayRef<i32> *>(p_other._data._array));

            if (l->array != r->array) {
                return false;
            }

            return true;
        } break;
        case INT64_ARRAY: {
            const ArrayRef<i64> *l = (reinterpret_cast<const ArrayRef<i64> *>(_data._array));
            const ArrayRef<i64> *r = (reinterpret_cast<const ArrayRef<i64> *>(p_other._data._array));

            if (l->array != r->array) {
                return false;
            }

            return true;
        } break;
        case FLOAT32_ARRAY: {
            const ArrayRef<float> *l = (reinterpret_cast<const ArrayRef<float> *>(_data._array));
            const ArrayRef<float> *r = (reinterpret_cast<const ArrayRef<float> *>(p_other._data._array));

            if (l->array != r->array) {
                return false;
            }

            return true;
        } break;
        case FLOAT64_ARRAY: {
            const ArrayRef<double> *l = (reinterpret_cast<const ArrayRef<double> *>(_data._array));
            const ArrayRef<double> *r = (reinterpret_cast<const ArrayRef<double> *>(p_other._data._array));

            if (l->array != r->array) {
                return false;
            }

            return true;
        } break;
        default:
            return false; //TODO: Maybe make this a little fairer?
    }
}

Variant::operator bool() const {
    switch (type) {
        case NIL:
            return false;
        case BOOL:
            return _data._bool;
        case INT:
            return _data._int ? true : false;
        case FLOAT:
            return _data._float ? true : false;
        default: {
            return false;
        }
    }
}


Variant::operator i8() const {
    switch (type) {
        case NIL:
            return 0;
        case BOOL:
            return _data._bool ? 1 : 0;
        case INT:
            return _data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0;
        }
    }
}

Variant::operator i16() const {
    switch (type) {
        case NIL:
            return 0;
        case BOOL:
            return _data._bool ? 1 : 0;
        case INT:
            return _data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0;
        }
    }
}

Variant::operator i32() const {
    switch (type) {
        case NIL:
            return 0;
        case BOOL:
            return _data._bool ? 1 : 0;
        case INT:
            return _data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0;
        }
    }
}

Variant::operator i64() const {
    switch (type) {
        case NIL:
            return 0;
        case BOOL:
            return _data._bool ? 1 : 0;
        case INT:
            return _data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0;
        }
    }
}

Variant::operator u8() const {
    switch (type) {
        case NIL:
            return 0;
        case BOOL:
            return _data._bool ? 1 : 0;
        case INT:
            return _data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0;
        }
    }
}

Variant::operator u16() const {
    switch (type) {
        case NIL:
            return 0;
        case BOOL:
            return _data._bool ? 1 : 0;
        case INT:
            return _data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0;
        }
    }
}

Variant::operator u32() const {
    switch (type) {
        case NIL:
            return 0;
        case BOOL:
            return _data._bool ? 1 : 0;
        case INT:
            return _data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0;
        }
    }
}

Variant::operator u64() const {
    switch (type) {
        case NIL:
            return 0;
        case BOOL:
            return _data._bool ? 1 : 0;
        case INT:
            return _data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0;
        }
    }
}


Variant::operator float() const {
    switch (type) {
        case NIL:
            return 0.0;
        case BOOL:
            return _data._bool ? 1.0 : 0.0;
        case INT:
            return (float)_data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0.0;
        }
    }
}

Variant::operator double() const {
    switch (type) {
        case NIL:
            return 0.0;
        case BOOL:
            return _data._bool ? 1.0 : 0.0;
        case INT:
            return (double)_data._int;
        case FLOAT:
            return _data._float;
        default: {
            return 0.0;
        }
    }
}

Variant::operator String() const {
    return stringify();
}

Variant::operator Array() const {
    if (type == ARRAY) {
        return *reinterpret_cast<const Array *>(_data._mem);
    } else {
        return Array();
    }
}

Variant::operator ByteArray() const {
    if (type == BYTE_ARRAY) {
        return static_cast<ArrayRef<u8> *>(_data._array)->array;
    } else {
        return Vector<u8>();
    }
}

Variant::operator Int32Array() const {
    if (type == INT32_ARRAY) {
        return static_cast<ArrayRef<i32> *>(_data._array)->array;
    } else {
        return Vector<i32>();
    }
}

Variant::operator Int64Array() const {
    if (type == INT64_ARRAY) {
        return static_cast<ArrayRef<i64> *>(_data._array)->array;
    } else {
        return Vector<i64>();
    }
}

Variant::operator Float32Array() const {
    if (type == FLOAT32_ARRAY) {
        return static_cast<ArrayRef<float> *>(_data._array)->array;
    } else {
        return Vector<float>();
    }
}

Variant::operator Float64Array() const {
    if (type == FLOAT64_ARRAY) {
        return static_cast<ArrayRef<double> *>(_data._array)->array;
    } else {
        return Vector<double>();
    }
}

Variant::Variant(i8 p_int) {
    _data._int = p_int;
    type = INT;
}

Variant::Variant(i16 p_int) {
    _data._int = p_int;
    type = INT;
}

Variant::Variant(i32 p_int) {
    _data._int = p_int;
    type = INT;
}

Variant::Variant(i64 p_int) {
    _data._int = p_int;
    type = INT;
}


Variant::Variant(u8 p_uint) {
    _data._int = p_uint;
    type = INT;
}


Variant::Variant(u16 p_uint) {
    _data._int = p_uint;
    type = INT;
}


Variant::Variant(u32 p_uint) {
    _data._int = p_uint;
    type = INT;
}


Variant::Variant(u64 p_uint) {
    _data._int = p_uint;
    type = INT;
}


Variant::Variant(bool p_bool) {
    _data._bool = p_bool;
    type = BOOL;
}


Variant::Variant(float p_float) {
    _data._float = p_float;
    type = FLOAT;
}


Variant::Variant(double p_double) {
    _data._float = p_double;
    type = FLOAT;
}


Variant::Variant(const String &p_string) {
    new (_data._mem) String(p_string);
    type = STRING;
}

Variant::Variant(const char *const p_cstring) {
    new (_data._mem) String(p_cstring);
    type = STRING;
}

Variant::Variant(const Array &p_array) {
    new (_data._mem) Array(p_array);
    type = ARRAY;
}

Variant::Variant(const ByteArray &p_byte_array) {
    _data._array = ArrayRef<u8>::create(p_byte_array);
    type = BYTE_ARRAY;
}

Variant::Variant(const Int32Array &p_int32_array) {
    _data._array = ArrayRef<i32>::create(p_int32_array);
    type = INT32_ARRAY;
}

Variant::Variant(const Int64Array &p_int64_array) {
    _data._array = ArrayRef<i64>::create(p_int64_array);
    type = INT64_ARRAY;
}

Variant::Variant(const Float32Array &p_float32_array) {
    _data._array = ArrayRef<float>::create(p_float32_array);
    type = FLOAT32_ARRAY;
}

Variant::Variant(const Float64Array &p_float64_array) {
    _data._array = ArrayRef<double>::create(p_float64_array);
    type = FLOAT64_ARRAY;
}