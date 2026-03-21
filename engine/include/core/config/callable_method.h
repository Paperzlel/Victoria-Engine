#pragma once

#include "core/data/atomic_counter.h"
#include "core/string/vstring.h"
#include "core/typedefs.h"
#include "core/variant/variant.h"

class CallableMethodBase;

class VAPI CallableMethod {
	CallableMethodBase *base = nullptr;

public:
	template <typename... Args>
	Error call(Args... p_args) const;
	void callp(const Variant **p_args, Error &r_error) const;

	String get_name() const;

	FORCE_INLINE bool is_valid() const {
		return base != nullptr;
	}
	FORCE_INLINE bool is_null() const {
		return base == nullptr;
	}

	void operator=(const CallableMethod &p_other);

	bool operator==(const CallableMethod &p_other);
	bool operator!=(const CallableMethod &p_other);

	CallableMethod() {}
	CallableMethod(CallableMethodBase *p_base);
	~CallableMethod();
};

class CallableMethodBase {
	friend class CallableMethod;
	String name;
	Refcount refcount;

public:
	void set_name(const String &p_name);

	virtual String get_name() const;
	virtual void call(const Variant **p_args) = 0;

	CallableMethodBase();
	virtual ~CallableMethodBase() {}
};

template <typename... Args>
Error CallableMethod::call(Args... p_args) const {
	Variant args[sizeof...(p_args) + 1] = {p_args..., Variant()};
	const Variant *argptrs[sizeof...(p_args) + 1];

	for (u32 i = 0; i < sizeof...(p_args); i++) {
		argptrs[i] = &args[i];
	}

	Error ret;
	callp(argptrs, ret);
	return ret;
}