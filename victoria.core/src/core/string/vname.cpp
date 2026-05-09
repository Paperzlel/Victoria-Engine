#include "core/string/vname.h"

#include "core/data/hashfuncs.h"

struct VName::Tree {
	static constexpr int TREE_SIZE = 1 << 16;
	static inline Data *tree_data[TREE_SIZE];
};

void VName::setup() {
	for (int i = 0; i < Tree::TREE_SIZE; i++) {
		Tree::tree_data[i] = nullptr;
	}
}

void VName::finalize() {
	uint32_t leaked_vnames = 0;
	for (int i = 0; i < Tree::TREE_SIZE; i++) {
		Data *_d = Tree::tree_data[i];
		if (_d) {
			if (_d->refcount.unref()) {
				vdelete(_d);
			} else {
				leaked_vnames++;
			}
		}

		Tree::tree_data[i] = nullptr;
	}

	if (leaked_vnames > 0) {
		ERR_WARN(vformat("%d VNames leaked at exit.", leaked_vnames));
	}
}

void VName::unref() {
	// Can free
	if (_data && _data->refcount.unref()) {
		if (_data->next) {
			_data->next->prev = _data->prev;
		}

		if (_data->prev) {
			_data->prev->next = _data->next;
		}

		uint32_t idx = _data->hash % Tree::TREE_SIZE;
		if (Tree::tree_data[idx] == _data) {
			Tree::tree_data[idx] = _data->next;
		}

		vdelete(_data);
	}

	_data = nullptr;
}

int32_t VName::length() const {
	if (_data) {
		return _data->string.length();
	}

	return 0;
}

const char *VName::get_data() const {
	if (_data) {
		return _data->string.get_data();
	}

	return nullptr;
}

char VName::operator[](int p_index) const {
	if (_data) {
		return _data->string[p_index];
	}

	return 0;
}

VName::VName(const String &p_other) {
	uint32_t h = HasherDefault::hash(p_other.get_data());
	if (h == 0) {
		return;
	}

	uint32_t idx = h % Tree::TREE_SIZE;
	Data *d = Tree::tree_data[idx];
	Data *pd = nullptr;
	while (d) {
		if (d->hash == h && d->string == p_other) {
			break;
		}

		pd = d;
		d = d->next;
	}

	if (d && d->refcount.ref()) {
		_data = d;
		return;
	}

	d = vnew(Data);
	d->hash = h;
	d->string = p_other;
	d->refcount.init();
	if (pd) {
		pd->next = d;
	}

	d->prev = pd;
	_data = d;
	if (!Tree::tree_data[idx]) {
		Tree::tree_data[idx] = _data;
	}
}

VName::VName(const char *p_string) {
	uint32_t h = HasherDefault::hash(p_string);
	if (h == 0) {
		return;
	}

	uint32_t idx = h % Tree::TREE_SIZE;
	Data *d = Tree::tree_data[idx];
	Data *pd = nullptr;
	while (d) {
		if (d->hash == h && d->string == p_string) {
			break;
		}

		pd = d;
		d = d->next;
	}

	if (d && d->refcount.ref()) {
		_data = d;
		return;
	}

	d = vnew(Data);
	d->hash = h;
	d->string = p_string;
	d->refcount.init();
	if (pd) {
		pd->next = d;
	}

	d->prev = pd;
	_data = d;
	if (!Tree::tree_data[idx]) {
		Tree::tree_data[idx] = _data;
	}
}

bool operator==(const String &p_left, const VName &p_right) {
	return p_right.operator==(p_left);
}

bool operator!=(const String &p_left, const VName &p_right) {
	return p_right.operator!=(p_left);
}

bool operator==(const char *p_left, const VName &p_right) {
	return p_right.operator==(p_left);
}

bool operator!=(const char *p_left, const VName &p_right) {
	return p_right.operator!=(p_left);
}
