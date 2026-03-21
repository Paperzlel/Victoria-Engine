#include "core/variant/array.h"

#include "core/data/atomic_counter.h"
#include "core/data/vector.h"
#include "core/variant/variant.h"

class ArrayData {
public:
	Vector<Variant> _array;
	bool read_only = false;
	Refcount ref_count;
};

void Array::_ref(const Array &p_from) const {
	ArrayData *_ad = p_from._data;

	ERR_COND_NULL(_ad);

	if (_ad == _data) {
		return;
	}

	ERR_FAIL_COND(!_ad->ref_count.ref());

	_unref();

	_data = _ad;
}

void Array::_unref() const {
	if (!_data) {
		return;
	}

	if (_data->ref_count.unref()) {
		vdelete(_data);
	}

	_data = nullptr;
}

Array::Iterator Array::begin() {
	return Iterator(_data->_array.ptrw());
}

Array::Iterator Array::end() {
	return Iterator(_data->_array.ptrw() + _data->_array.size());
}

Array::ConstIterator Array::begin() const {
	return ConstIterator(_data->_array.ptr());
}

Array::ConstIterator Array::end() const {
	return ConstIterator(_data->_array.ptr() + _data->_array.size());
}

Variant &Array::operator[](uint64_t p_index) {
	return _data->_array[p_index];
}

const Variant &Array::operator[](uint64_t p_index) const {
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

int64_t Array::size() const {
	return _data->_array.size();
}

Error Array::resize(int p_new_size) {
	return _data->_array.resize(p_new_size);
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

void Array::append(const Variant &p_item) {
	push_back(p_item);
}

void Array::remove_at(uint64_t p_index) {
	ERR_FAIL_COND_MSG(_data->_array.is_empty(), "Cannot remove an item from an empty array.");
	ERR_FAIL_COND_MSG(_data->read_only, "Can't remove elements from a read-only array.");
	_data->_array.remove_at(p_index);
}

void Array::insert(uint64_t p_index, const Variant &p_item) {
	ERR_FAIL_COND_MSG(_data->read_only, "Can't add elements to a read-only array.");
	Variant value = p_item;
	_data->_array.insert_at(value, p_index);
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

void Array::push_front(const Variant &p_item) {
	ERR_FAIL_COND_MSG(_data->read_only, "Can't add elements to a read-only array.");
	Variant value = p_item;
	_data->_array.push_front(value);
}

void Array::push_back(const Variant &p_item) {
	ERR_FAIL_COND_MSG(_data->read_only, "Can't add elements to a read-only array.");
	Variant value = p_item;
	_data->_array.push_back(value);
}

void Array::clear() {
	ERR_FAIL_COND_MSG(_data->_array.is_empty(), "Cannot clear an empty array.");
	ERR_FAIL_COND_MSG(_data->read_only, "Can't remove elements from a read-only array.");
	_data->_array.clear();
}

void Array::fill(const Variant &p_item) {
	ERR_FAIL_COND_MSG(_data->_array.is_empty(), "Cannot fill an empty array.");
	Variant value = p_item;
	for (int i = 0; i < _data->_array.size(); i++) {
		_data->_array[i] = value;
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

Array::Array(const Array &p_from) {
	_data = nullptr;
	_ref(p_from);
}

Array::Array() {
	_data = vnew(ArrayData);
	_data->ref_count.set(1);
}

Array::~Array() {
	_unref();
}