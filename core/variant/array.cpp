#include "array.h"

#include "core/data/vector.h"
#include "core/variant/variant.h"

class ArrayData {
public:
    Vector<Variant> _array;
    bool read_only = false;
    // HACK: this variable really shouldn't need to exist (something like Godot's RefCounted should be used instead) but this allows us to double check if a block has been deleted and so prevent a heap corruption.
    bool freed = false;
};


void Array::_ref(const Array &p_from) const {
    ArrayData *_ad = p_from._data;
    
    ERR_COND_NULL(_ad);

    if (_ad == _data) {
        return;
    }

    _unref();

    _data = _ad;
}

void Array::_unref() const {
    if (!_data) {
        return;
    }
    
    // Already freed the data, no need to do it again.
    if (_data->freed) {
        _data = nullptr;
        return;
    }

    delete _data;

    // the fact that I can still assign after delete makes no sense but I'll take it.
    _data->freed = true;
    _data = nullptr;
}

Array::Iterator Array::begin() {
    return Iterator(_data->_array.ptrw());
}

Array::Iterator Array::end() {
    return Iterator(_data->_array.ptrw() + _data->_array.size());
}

Variant &Array::operator[](uint64 p_index) {
    return _data->_array[p_index];
}

const Variant &Array::operator[](uint64 p_index) const {
    return _data->_array[p_index];
}

void Array::operator=(const Array &p_other) {
    if (this == &p_other) {
        return;
    }

    _ref(p_other);
}

bool Array::operator==(const Array &p_other) {
    return is_equal(p_other);
}

bool Array::operator!=(const Array &p_other) {
    return !is_equal(p_other);
}

uint64 Array::size() const {
    return _data->_array.size();
}

bool Array::is_empty() const {
    return _data->_array.is_empty();
}

bool Array::is_read_only() const {
    return _data->read_only;
}

void Array::set_read_only(bool p_bool) const {
    _data->read_only = p_bool;
}

void Array::append(Variant p_item) {
   push_back(p_item);
}

void Array::remove_at(uint64 p_index) {
    ERR_FAIL_COND_MSG(_data->_array.is_empty(), "Cannot remove an item from an empty array.");
    ERR_FAIL_COND_MSG(_data->read_only, "Can't remove elements from a read-only array.");
    _data->_array.remove_at(p_index);
}

void Array::insert(uint64 p_index, Variant &p_item) {
    ERR_FAIL_COND_MSG(_data->read_only, "Can't add elements to a read-only array.");
    _data->_array.insert_at(p_item, p_index);
}

Variant Array::pop_front() {
    ERR_FAIL_COND_MSG_R(_data->_array.is_empty(), "Cannot remove an item from an empty array.", Variant());
    ERR_FAIL_COND_MSG_R(_data->read_only, "Can't remove elements from a read-only array.", Variant());
    return _data->_array.pop_front();
}

Variant Array::pop_back() {
    ERR_FAIL_COND_MSG_R(_data->_array.is_empty(), "Cannot remove an item from an empty array.", Variant());
    ERR_FAIL_COND_MSG_R(_data->read_only, "Can't remove elements from a read-only array.", Variant());
    return _data->_array.pop_back();
}

void Array::push_front(Variant p_item) {
    ERR_FAIL_COND_MSG(_data->read_only, "Can't add elements to a read-only array.");
    _data->_array.push_front(p_item);
}

void Array::push_back(Variant p_item) {
    ERR_FAIL_COND_MSG(_data->read_only, "Can't add elements to a read-only array.");
    _data->_array.push_back(p_item);
}

void Array::clear() {
    ERR_FAIL_COND_MSG(_data->_array.is_empty(), "Cannot clear an empty array.");
    ERR_FAIL_COND_MSG(_data->read_only, "Can't remove elements from a read-only array.");
    _data->_array.clear();
}

void Array::fill(Variant p_item) {
    ERR_FAIL_COND_MSG(_data->_array.is_empty(), "Cannot fill an empty array.");
    for (int i = 0; i < _data->_array.size(); i++) {
        _data->_array[i] = p_item;
    }
}

bool Array::is_equal(const Array &p_other) const {
    if (_data == p_other._data) {
        return true;
    }

    const Vector<Variant> &a1 = _data->_array;
    const Vector<Variant> &a2 = p_other._data->_array;

    const int size = a1.size();

    if (size != a2.size()) {
        return false;
    }

    for (int i = 0; i < size; i++) {
        if (!a1[i].hash_compare(a2[i], 0)) {
            return false;
        }
    }

    return true;
}



Array::Array(Array &p_from) {
    _data = nullptr;
    _ref(p_from);
}

Array::Array() {
    // NOTE: This new allocation only lasts for a singular Array - this means that when we copy one to the other, the ArrayData will still be in use elsewhere prior to being freed. Because of this, we need to have our "freed" variable to let us know if the data has been handled properly
    _data = new ArrayData;
}

Array::~Array() {
    _unref();
}