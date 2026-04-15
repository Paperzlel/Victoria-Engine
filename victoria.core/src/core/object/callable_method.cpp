#include "core/object/callable_method.h"

void CallableMethod::callp(const Variant **p_args, Error &r_error) const {
	r_error = OK;
	base->call(p_args == nullptr ? nullptr : p_args);
}

String CallableMethod::get_name() const {
	if (base) {
		return base->get_name();
	}
	return "";
}

void CallableMethod::operator=(const CallableMethod &p_other) {
	CallableMethodBase *cleanup = nullptr;
	if (base == p_other.base) {
		return;
	}

	cleanup = base;
	base = nullptr;

	if (p_other.base->refcount.ref()) {
		base = p_other.base;
	}

	if (cleanup != nullptr && cleanup->refcount.unref()) {
		vdelete(cleanup);
	}

	cleanup = nullptr;
}

bool CallableMethod::operator==(const CallableMethod &p_other) {
	if (base == p_other.base) {
		return true;
	}

	return false;
}

bool CallableMethod::operator!=(const CallableMethod &p_other) {
	if (base == p_other.base) {
		return false;
	}

	return true;
}

CallableMethod::CallableMethod(CallableMethodBase *p_base) {
	base = p_base;
}

CallableMethod::~CallableMethod() {
	if (base && base->refcount.unref()) {
		vdelete(base);
		base = nullptr;
	}
}

CallableMethodBase::CallableMethodBase() {
	refcount.init();
}

String CallableMethodBase::get_name() const {
	return name;
}

void CallableMethodBase::set_name(const String &p_name) {
	name = p_name;
}