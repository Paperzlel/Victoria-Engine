#pragma once

#include "core/data/atomic_counter.h"
#include "core/string/vstring.h"
#include "core/typedefs.h"
#include "core/variant/variant.h"

class CallableMethodBase;

class VAPI CallableMethod {
	CallableMethodBase *base = nullptr;

public:
	/**
	 * @brief Calls the method bound to this callable with the passed arguments.
	 * @param p_args Variadic list of arguments that are passed to the function. Currently, there is no way to verify
	 * that this argument count aligns with the number expected internally, which can lead to undefined behaviour.
	 * Additionally, non-Variant arguments cannot be passed as there is no conversion to and from these types at the
	 * call stage.
	 * @return `OK` on success, and various error messages if not.
	 */
	template <typename... Args>
	Error call(Args... p_args) const;

	/**
	 * @brief Calls the method bound to this callable with the passed arguments. This method is the interal call used
	 * by `call` and should be avoided by end-users.
	 * @param p_args Array of pointers to arguments that are passed to the function. Currently, there is no way to
	 * verify that this argument count aligns with the number expected internally, which can lead to undefined
	 * behaviour. Additionally, non-Variant arguments cannot be passed as there is no conversion to and from these
	 * types at the call stage.
	 * @param r_error The return error to pass back once called. `OK` on success.
	 */
	void callp(const Variant **p_args, Error &r_error) const;

	/**
	 * @brief Calls the current method later on in the process queue, usually at the end of the process frame in a
	 * typical application.
	 * @param p_args A varadic list of arguments to call the function with when it is time to be called. Currently,
	 * there is no way to verify that this argument count aligns with the number expected internally, which can lead to
	 * undefined behaviour. Additionally, non-Variant arguments cannot be passed as there is no conversion to and from
	 * these types at the call stage.
	 * @return `OK` if the event was able to be queued, and an error code if not.
	 */
	template <typename... Args>
	Error call_deferred(Args... p_args) const;

	/**
	 * @brief Calls the current method later on in the process queue, usually at the end of the process frame in a
	 * typical application. This method is a pointer version of `call_deferred` and should be avoided in most
	 * circumstances.
	 * @param p_args An array of pointers to arguments to call the function with when it is time to be called.
	 * Currently, there is no way to verify that this argument count aligns with the number expected internally, which
	 * can lead to undefined behaviour. Additionally, non-Variant arguments cannot be passed as there is no conversion
	 * to and from these types at the call stage.
	 * @param p_argc The number of arguments in the pointer array, used for decoding at the end of the array.
	 * @param r_error The return error for the function, which is `OK` on success.
	 */
	void call_deferredp(const Variant **p_args, int p_argc, Error &r_error) const;

	/**
	 * @brief Obtains the name of the function.
	 * @return The name of the function.
	 */
	String get_name() const;

	/**
	 * @brief Checks to see if the given callable is valid to be used. If not, calling the function will produce a
	 * segmentation fault.
	 * @return `true` if non-null, and `false` if not.
	 */
	FORCE_INLINE bool is_valid() const {
		return base != nullptr;
	}

	/**
	 * @brief Checks to see if the givne callable is null. If null, it should not be called. Inverse of `is_valid`.
	 * @return `true` if null, and `false` if not.
	 */
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

/**
 * @brief Implementation of `CallableMethod` data so that functionality referring to different types of methods can be
 * implemented and access the same.
 */
class VAPI CallableMethodBase {
	friend class CallableMethod;
	String name;	   // The name of the function. Will be in the form `<class_name>::<function_name>`.
	Refcount refcount; // The reference count to the current method base. Bases are transferred between callables to
					   // save on memory allocations and consistency

public:
	/**
	 * @brief Sets the name of the function. Shouldn't be called outside of the constructor, but for implementation
	 * reasons needs to be public.
	 * @param p_name The name of the function, usually derived from the function pointer passed during creation.
	 */
	void set_name(const String &p_name);

	/**
	 * @brief Virtual name-getting function. Can be overridden in later implementations.
	 * @return The name of the method.
	 */
	virtual String get_name() const;

	/**
	 * @brief Calls the bound method. Abstract on the base as this needs to be overridden for each implementation, as
	 * static function pointers differ to non-static ones and the like.
	 * @param p_args Array of argument pointers to pass into the function.
	 */
	virtual void call(const Variant **p_args) = 0;

	CallableMethodBase();
	virtual ~CallableMethodBase() {}
};

template <typename... Args>
Error CallableMethod::call(Args... p_args) const {
	Variant args[sizeof...(p_args) + 1] = {p_args..., Variant()};
	const Variant *argptrs[sizeof...(p_args) + 1];

	for (uint32_t i = 0; i < sizeof...(p_args); i++) {
		argptrs[i] = &args[i];
	}

	Error ret = OK;
	callp(argptrs, ret);
	return ret;
}

template <typename... Args>
Error CallableMethod::call_deferred(Args... p_args) const {
	Variant args[sizeof...(p_args) + 1] = {p_args..., Variant()};
	const Variant *argptrs[sizeof...(p_args) + 1];

	for (uint32_t i = 0; i < sizeof...(p_args); i++) {
		argptrs[i] = &args[i];
	}

	Error ret = OK;
	call_deferredp(argptrs, sizeof...(p_args), ret);
	return ret;
}
