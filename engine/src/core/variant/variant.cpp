#include "core/variant/variant.h"

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
        case VECTOR2_ARRAY: {
            ArrayRefBase::destroy(_data._array);
        } break;
        case VECTOR3_ARRAY: {
            ArrayRefBase::destroy(_data._array);
        } break;
        case VECTOR4_ARRAY: {
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

template <typename T>
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

String Variant::stringify(int recursion_count) const {
    switch (type) {
        case NIL: 
            return "<null>";
        case BOOL: 
            return _data._bool ? "true" : "false";
        case INT:
            return itos(_data._int);
        case FLOAT: 
            return ftos(_data._float);
        case STRING:
            return *reinterpret_cast<const String *>(_data._mem);
        case VECTOR2: 
            return operator Vector2();
        case VECTOR2I:
            return operator Vector2i();
        case VECTOR3:
            return operator Vector3();
        case VECTOR3I:
            return operator Vector3i();
        case VECTOR4:
            return operator Vector4();
        case VECTOR4I:
            return operator Vector4i();
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
        case VECTOR2_ARRAY:
            return stringify_vector(operator Vector2Array(), recursion_count);
        case VECTOR3_ARRAY:
            return stringify_vector(operator Vector3Array(), recursion_count);
        case VECTOR4_ARRAY:
            return stringify_vector(operator Vector4Array(), recursion_count);
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
            vnew_placement(_data._mem, String(*reinterpret_cast<const String *>(p_other._data._mem)));
        } break;
        case VECTOR2: {
            vnew_placement(_data._mem, Vector2(*reinterpret_cast<const Vector2 *>(p_other._data._mem)));
        } break;
        case VECTOR2I: {
            vnew_placement(_data._mem, Vector2i(*reinterpret_cast<const Vector2i *>(p_other._data._mem)));
        } break;
        case VECTOR3: {
            vnew_placement(_data._mem, Vector3(*reinterpret_cast<const Vector3 *>(p_other._data._mem)));
        } break;
        case VECTOR3I: {
            vnew_placement(_data._mem, Vector3i(*reinterpret_cast<const Vector3i *>(p_other._data._mem)));
        } break;
        case VECTOR4: {
            vnew_placement(_data._mem, Vector4(*reinterpret_cast<const Vector4 *>(p_other._data._mem)));
        } break;
        case VECTOR4I: {
            vnew_placement(_data._mem, Vector4i(*reinterpret_cast<const Vector4i *>(p_other._data._mem)));
        } break;

        case ARRAY: {
            vnew_placement(_data._mem, Array(*reinterpret_cast<const Array *>(p_other._data._mem)));
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
        } break;
        case VECTOR2_ARRAY: {
            _data._array = static_cast<ArrayRef<Vector2> *>(p_other._data._array)->reference();
            if (!_data._array) {
                _data._array = ArrayRef<Vector2>::create();
            }
        } break;
        case VECTOR3_ARRAY: {
            _data._array = static_cast<ArrayRef<Vector3> *>(p_other._data._array)->reference();
            if (!_data._array) {
                _data._array = ArrayRef<Vector3>::create();
            }
        } break;
        case VECTOR4_ARRAY: {
            _data._array = static_cast<ArrayRef<Vector4> *>(p_other._data._array)->reference();
            if (!_data._array) {
                _data._array = ArrayRef<Vector4>::create();
            }
        } break;
        default: {
        }
    }
}

void Variant::operator=(const Variant &p_var) {

    if (type != p_var.type) {
        _ref(p_var);
        return;
    }
    type = p_var.type;

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
        case VECTOR2: {
            *reinterpret_cast<Vector2 *>(_data._mem) = *reinterpret_cast<const Vector2 *>(p_var._data._mem);
        } break;
        case VECTOR2I: {
            *reinterpret_cast<Vector2i *>(_data._mem) = *reinterpret_cast<const Vector2i *>(p_var._data._mem);
        } break;
        case VECTOR3: {
            *reinterpret_cast<Vector3 *>(_data._mem) = *reinterpret_cast<const Vector3 *>(p_var._data._mem);
        } break;
        case VECTOR3I: {
            *reinterpret_cast<Vector3i *>(_data._mem) = *reinterpret_cast<const Vector3i *>(p_var._data._mem);
        } break;
        case VECTOR4: {
            *reinterpret_cast<Vector4 *>(_data._mem) = *reinterpret_cast<const Vector4 *>(p_var._data._mem);
        } break;
        case VECTOR4I: {
            *reinterpret_cast<Vector4i *>(_data._mem) = *reinterpret_cast<const Vector4i *>(p_var._data._mem);
        } break;
        // NOTE: We use reinterpret_cast because the various datatypes that exist share the same memory area, therefore we need to set it to an array pointer before we copy in the data to its location. If the data isn't an array pointer, we can't use it in its current form and so the casting needs to happen first.
        case ARRAY: {
            *reinterpret_cast<Array *>(_data._mem) = *reinterpret_cast<const Array *>(p_var._data._mem);
        } break;
        default:
            break;
    } 
}

bool Variant::operator==(const Variant &other) const {
    return hash_compare(other, 0);
}

bool Variant::operator!=(const Variant &other) const {
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
        case ARRAY: {
            const Array &l = *(reinterpret_cast<const Array *>(_data._mem));
            const Array &r = *(reinterpret_cast<const Array *>(p_other._data._mem));
            if (!l.is_equal(r)) {
                return false;
            }

            return true;
        } break;
        case STRING: {
            const String &l = *(reinterpret_cast<const String *>(_data._mem));
            const String &r = *(reinterpret_cast<const String *>(p_other._data._mem));
            if (l != r) {
                return false;
            }

            return true;
        } break;
        default:
            return false;
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

Variant::operator Vector2() const {
    switch (type) {
        case VECTOR2: {
            return *reinterpret_cast<const Vector2 *>(_data._mem);
        };
        case VECTOR2I: {
            return *reinterpret_cast<const Vector2i *>(_data._mem);   
        };
        case VECTOR3: {
            return Vector2(reinterpret_cast<const Vector3 *>(_data._mem)->x, reinterpret_cast<const Vector3 *>(_data._mem)->y);
        };
        case VECTOR3I: {
            return Vector2(reinterpret_cast<const Vector3i *>(_data._mem)->x, reinterpret_cast<const Vector3i *>(_data._mem)->y);
        };
        case VECTOR4: {
            return Vector2(reinterpret_cast<const Vector4 *>(_data._mem)->x, reinterpret_cast<const Vector4 *>(_data._mem)->y);
        };
        case VECTOR4I: {
            return Vector2(reinterpret_cast<const Vector4i *>(_data._mem)->x, reinterpret_cast<const Vector4i *>(_data._mem)->y);
        };
        default: {
            return Vector2();
        };
    }
}

Variant::operator Vector2i() const {
    switch (type) {
        case VECTOR2: {
            return *reinterpret_cast<const Vector2 *>(_data._mem);
        };
        case VECTOR2I: {
            return *reinterpret_cast<const Vector2i *>(_data._mem);   
        };
        case VECTOR3: {
            return Vector2i(reinterpret_cast<const Vector3 *>(_data._mem)->x, reinterpret_cast<const Vector3 *>(_data._mem)->y);
        };
        case VECTOR3I: {
            return Vector2i(reinterpret_cast<const Vector3i *>(_data._mem)->x, reinterpret_cast<const Vector3i *>(_data._mem)->y);
        };
        case VECTOR4: {
            return Vector2i(reinterpret_cast<const Vector4 *>(_data._mem)->x, reinterpret_cast<const Vector4 *>(_data._mem)->y);
        };
        case VECTOR4I: {
            return Vector2i(reinterpret_cast<const Vector4i *>(_data._mem)->x, reinterpret_cast<const Vector4i *>(_data._mem)->y);
        };
        default: {
            return Vector2i();
        };
    }
}

Variant::operator Vector3() const {
    switch (type) {
        case VECTOR2: {
            return Vector3(reinterpret_cast<const Vector2 *>(_data._mem)->x, reinterpret_cast<const Vector2 *>(_data._mem)->y, 0.0);
        };
        case VECTOR2I: {
            return Vector3(reinterpret_cast<const Vector2i *>(_data._mem)->x, reinterpret_cast<const Vector2i *>(_data._mem)->y, 0.0);
        };
        case VECTOR3: {
            return *reinterpret_cast<const Vector3 *>(_data._mem);
        };
        case VECTOR3I: {
            return *reinterpret_cast<const Vector3i *>(_data._mem);
        };
        case VECTOR4: {
            return Vector3(reinterpret_cast<const Vector4 *>(_data._mem)->x, reinterpret_cast<const Vector4 *>(_data._mem)->y, reinterpret_cast<const Vector4 *>(_data._mem)->z);
        };
        case VECTOR4I: {
            return Vector3(reinterpret_cast<const Vector4i *>(_data._mem)->x, reinterpret_cast<const Vector4i *>(_data._mem)->y, reinterpret_cast<const Vector4i *>(_data._mem)->z);
        };
        default: {
            return Vector3();
        }
    }
}

Variant::operator Vector3i() const {
    switch (type) {
        case VECTOR2: {
            return Vector3i(reinterpret_cast<const Vector2 *>(_data._mem)->x, reinterpret_cast<const Vector2 *>(_data._mem)->y, 0);
        };
        case VECTOR2I: {
            return Vector3i(reinterpret_cast<const Vector2i *>(_data._mem)->x, reinterpret_cast<const Vector2i *>(_data._mem)->y, 0);
        };
        case VECTOR3: {
            return *reinterpret_cast<const Vector3 *>(_data._mem);
        };
        case VECTOR3I: {
            return *reinterpret_cast<const Vector3i *>(_data._mem);
        };
        case VECTOR4: {
            return Vector3i(reinterpret_cast<const Vector4 *>(_data._mem)->x, reinterpret_cast<const Vector4 *>(_data._mem)->y, reinterpret_cast<const Vector4 *>(_data._mem)->z);
        };
        case VECTOR4I: {
            return Vector3i(reinterpret_cast<const Vector4i *>(_data._mem)->x, reinterpret_cast<const Vector4i *>(_data._mem)->y, reinterpret_cast<const Vector4i *>(_data._mem)->z);
        };
        default: {
            return Vector3i();
        }
    }
}

Variant::operator Vector4() const {
    switch (type) {
        case VECTOR2: {
            return Vector4(reinterpret_cast<const Vector2 *>(_data._mem)->x, reinterpret_cast<const Vector2 *>(_data._mem)->y, 0.0, 0.0);
        };
        case VECTOR2I: {
            return Vector4(reinterpret_cast<const Vector2i *>(_data._mem)->x, reinterpret_cast<const Vector2i *>(_data._mem)->y, 0.0, 0.0);
        };
        case VECTOR3: {
            return Vector4(reinterpret_cast<const Vector3 *>(_data._mem)->x, reinterpret_cast<const Vector3 *>(_data._mem)->y, reinterpret_cast<const Vector3 *>(_data._mem)->z, 0.0);
        };
        case VECTOR3I: {
            return Vector4(reinterpret_cast<const Vector3i *>(_data._mem)->x, reinterpret_cast<const Vector3i *>(_data._mem)->y, reinterpret_cast<const Vector3i *>(_data._mem)->z, 0.0);
        };
        case VECTOR4: {
            return *reinterpret_cast<const Vector4 *>(_data._mem);
        };
        case VECTOR4I: {
            return *reinterpret_cast<const Vector4i *>(_data._mem);
        };
        default: {
            return Vector4();
        }
    }
}

Variant::operator Vector4i() const {
    switch (type) {
        case VECTOR2: {
            return Vector4i(reinterpret_cast<const Vector2 *>(_data._mem)->x, reinterpret_cast<const Vector2 *>(_data._mem)->y, 0, 0);
        };
        case VECTOR2I: {
            return Vector4i(reinterpret_cast<const Vector2i *>(_data._mem)->x, reinterpret_cast<const Vector2i *>(_data._mem)->y, 0, 0);
        };
        case VECTOR3: {
            return Vector4i(reinterpret_cast<const Vector3 *>(_data._mem)->x, reinterpret_cast<const Vector3 *>(_data._mem)->y, reinterpret_cast<const Vector3 *>(_data._mem)->z, 0);
        };
        case VECTOR3I: {
            return Vector4i(reinterpret_cast<const Vector3i *>(_data._mem)->x, reinterpret_cast<const Vector3i *>(_data._mem)->y, reinterpret_cast<const Vector3i *>(_data._mem)->z, 0);
        };
        case VECTOR4: {
            return *reinterpret_cast<const Vector4 *>(_data._mem);
        };
        case VECTOR4I: {
            return *reinterpret_cast<const Vector4i *>(_data._mem);
        };
        default: {
            return Vector4i();
        }
    }
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

Variant::operator Vector2Array() const {
    if (type == VECTOR2_ARRAY) {
        return static_cast<ArrayRef<Vector2> *>(_data._array)->array;
    } else {
        return Vector<Vector2>();
    }
}

Variant::operator Vector3Array() const {
    if (type == VECTOR3_ARRAY) {
        return static_cast<ArrayRef<Vector3> *>(_data._array)->array;
    } else {
        return Vector<Vector3>();
    }
}

Variant::operator Vector4Array() const {
    if (type == VECTOR4_ARRAY) {
        return static_cast<ArrayRef<Vector4> *>(_data._array)->array;
    } else {
        return Vector<Vector4>();
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

/**
 * @brief Variant class constructor, unsigned 8-bit integer version
 * @param p_uint An unsigned 8-bit integer
 */
Variant::Variant(u8 p_uint) {
    _data._int = p_uint;
    type = INT;
}

/**
 * @brief Variant class constructor, unsigned 16-bit integer version
 * @param p_uint An unsigned 16-bit integer
 */
Variant::Variant(u16 p_uint) {
    _data._int = p_uint;
    type = INT;
}

/**
 * @brief Variant class constructor, unsigned 32-bit integer version
 * @param p_uint An unsigned 32-bit integer
 */
Variant::Variant(u32 p_uint) {
    _data._int = p_uint;
    type = INT;
}

/**
 * @brief Variant class constructor, unsigned 64-bit integer version
 * @param p_uint An unsigned 64-bit integer
 */
Variant::Variant(u64 p_uint) {
    _data._int = p_uint;
    type = INT;
}

/**
 * @brief Variant class constructor, boolean version
 * @param p_bool A boolean
 */
Variant::Variant(bool p_bool) {
    _data._bool = p_bool;
    type = BOOL;
}

/**
 * @brief Variant class constructor, 32-bit float version
 * @param p_float A 32-bit float
 */
Variant::Variant(float p_float) {
    _data._float = p_float;
    type = FLOAT;
}

/**
 * @brief Variant class constructor, 64-bit float version
 * @param p_double A 64-bit float
 */
Variant::Variant(double p_double) {
    _data._float = p_double;
    type = FLOAT;
}

Variant::Variant(const String &p_string) {
    vnew_placement(_data._mem, String(p_string));
    type = STRING;
}

Variant::Variant(const char *const p_cstring) {
    vnew_placement(_data._mem, String(p_cstring));
    type = STRING;
}

Variant::Variant(const Vector2 &p_vec) {
    vnew_placement(_data._mem, Vector2(p_vec));
    type = VECTOR2;
}

Variant::Variant(const Vector2i &p_vec) {
    vnew_placement(_data._mem, Vector2i(p_vec));
    type = VECTOR2I;
}

Variant::Variant(const Vector3 &p_vec) {
    vnew_placement(_data._mem, Vector3(p_vec));
    type = VECTOR3;
}

Variant::Variant(const Vector3i &p_vec) {
    vnew_placement(_data._mem, Vector3i(p_vec));
    type = VECTOR3I;
}

Variant::Variant(const Vector4 &p_vec) {
    vnew_placement(_data._mem, Vector4(p_vec));
    type = VECTOR4;
}

Variant::Variant(const Vector4i &p_vec) {
    vnew_placement(_data._mem, Vector4i(p_vec));
    type = VECTOR4I;
}

Variant::Variant(const Array &p_array) {
    vnew_placement(_data._mem, Array(p_array));
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

Variant::Variant(const Vector2Array &p_vector2_array) {
    _data._array = ArrayRef<Vector2>::create(p_vector2_array);
    type = VECTOR2_ARRAY;
}

Variant::Variant(const Vector3Array &p_vector3_array) {
    _data._array = ArrayRef<Vector3>::create(p_vector3_array);
    type = VECTOR3_ARRAY;
}

Variant::Variant(const Vector4Array &p_vector4_array) {
    _data._array = ArrayRef<Vector4>::create(p_vector4_array);
}
