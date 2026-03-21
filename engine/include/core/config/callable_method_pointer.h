#pragma once

#include "callable_method.h"

#include "core/typedefs.h"
#include "core/variant/variant_caster.h"

template <typename C, typename... Args, uint64_t... Is>
void method_call_varargs(C *p_class, void (C::*p_method)(Args...), const Variant **p_args, Indicies<Is...>) {
	(p_class->*p_method)(VariantCaster<Args>::cast(*p_args[Is])...);
}

template <typename... Args, uint64_t... Is>
void static_method_call_varargs(void (*p_method)(Args...), const Variant **p_args, Indicies<Is...>) {
	p_method(VariantCaster<Args>::cast(*p_args[Is])...);
}

template <typename C, typename... Args>
class CallableMethodPointer : public CallableMethodBase {
	C *instance_ptr = nullptr;
	void (C::*method_ptr)(Args...) = nullptr;

public:
	virtual String get_name() const override {
		return CallableMethodBase::get_name();
	}

	virtual void call(const Variant **p_args) override {
		method_call_varargs(instance_ptr, method_ptr, p_args, BuildIndicies<sizeof...(Args)>{});
	}

	CallableMethodPointer(C *p_instance, const char *p_name, void (C::*p_method)(Args...)) {
		instance_ptr = p_instance;
		method_ptr = p_method;
	}
};

template <typename C, typename... Args>
CallableMethod create_callable_method_pointer(C *p_instance, const char *p_name, void (C::*p_method)(Args...)) {
	typedef CallableMethodPointer<C, Args...> CMP;
	CMP *cmp = vnew(CMP(p_instance, p_name, p_method));
	cmp->set_name(p_name + 1);
	return CallableMethod(cmp);
}

#define callable_mp(m_c, m_p) create_callable_method_pointer(m_c, #m_p, m_p)

template <typename... Args>
class CallableMethodPointerStatic : public CallableMethodBase {
	void (*method_ptr)(Args...) = nullptr;

public:
	virtual String get_name() const override {
		return CallableMethodBase::get_name();
	}

	virtual void call(const Variant **p_args) override {
		static_method_call_varargs(method_ptr, p_args, BuildIndicies<sizeof...(Args)>{});
	}

	CallableMethodPointerStatic(const char *p_name, void (*p_method)(Args...)) {
		method_ptr = p_method;
	}
};

template <typename... Args>
CallableMethod create_static_callable_method_pointer(const char *p_name, void (*p_method)(Args...)) {
	typedef CallableMethodPointerStatic<Args...> CMPS;
	CMPS *cmp = vnew(CMPS(p_name, p_method));
	cmp->set_name(p_name + 1);
	return CallableMethod(cmp);
}

#define static_callable_mp(m_n) create_static_callable_method_pointer(#m_n, m_n)
