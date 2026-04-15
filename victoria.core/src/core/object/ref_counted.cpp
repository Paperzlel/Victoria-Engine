#include "core/object/ref_counted.h"

bool RefCounted::ref_init() {
	if (reference()) {
		if (!is_referenced() && init_rc.unref()) {
			unreference();
		}

		return true;
	} else {
		return false;
	}
}

bool RefCounted::reference() {
	bool can_ref = refcount.ref();

	return can_ref;
}

bool RefCounted::unreference() {
	bool can_die = refcount.unref();

	return can_die;
}

uint64_t RefCounted::get_refcount() const {
	return refcount.get();
}

RefCounted::RefCounted() {
	refcount.init();
	init_rc.init();
}

RefCounted::~RefCounted() {}