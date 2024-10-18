#include "variant.h"

void Variant::_clear_internals() {
    switch (type) {
        default: {

        } break;
    }
}

const char *Variant::get_value_as_string() {
    switch (type) {
        case NIL: 
            return "<null>";
        case BOOL: 
            return _data._bool ? "true" : "false";
        case INT:
        case FLOAT: 
            return ""; //TODO: handle
        default: 
            return "";
    }
}

void Variant::operator=(const Variant &p_var) {
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


Variant::operator int8() const {
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

Variant::operator int16() const {
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

Variant::operator int32() const {
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

Variant::operator int64() const {
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

Variant::operator uint64() const {
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

Variant::operator uint32() const {
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

Variant::operator uint16() const {
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

Variant::operator uint8() const {
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

Variant::Variant(int8 p_int) {
    _data._int = p_int;
    type = INT;
}

Variant::Variant(int16 p_int) {
    _data._int = p_int;
    type = INT;
}

Variant::Variant(int32 p_int) {
    _data._int = p_int;
    type = INT;
}

Variant::Variant(int64 p_int) {
    _data._int = p_int;
    type = INT;
}


Variant::Variant(uint8 p_uint) {
    _data._int = p_uint;
    type = INT;
}


Variant::Variant(uint16 p_uint) {
    _data._int = p_uint;
    type = INT;
}


Variant::Variant(uint32 p_uint) {
    _data._int = p_uint;
    type = INT;
}


Variant::Variant(uint64 p_uint) {
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
