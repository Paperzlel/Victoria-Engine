#pragma once

#include "item.h"

#include "core/data/atomic_counter.h"
#include "core/typedefs.h"

/**
 * @brief Base class for all refcounted objects. Ref-counted, so that multiple bits of code can call to the same region
 * of memory.
 */
class VAPI Instance : public Item {
	VREGISTER_CLASS(Instance, Item);

	// The reference counter
	Refcount refcount;
	// Initialization reference count
	Refcount init_rc;

public:
	// references

	/**
	 * @brief Initializes the ref-count.
	 * @returns True if the ref-counter was set successfully, and false if not.
	 */
	bool ref_init();

	/**
	 * @brief Increments the reference counter whenever it is referenced in code.
	 * @returns True if successful, false if not.
	 */
	bool reference();

	/**
	 * @brief Decrements the reference counter whenever a reference is no longer used.
	 * @returns True if the refcount is now 0 and can be freed, and false if not.
	 */
	bool unreference();

	/**
	 * @brief Obtains the number of references to the given object.
	 * @returns The reference count.
	 */
	uint64_t get_refcount() const;

	/**
	 * @brief Checks to see if the refcount has been initialized.
	 * @returns True if it is initialized, and false if not.
	 */
	bool is_referenced() const {
		return init_rc.get() != 1;
	}

	/**
	 * @brief Instance class constructor.
	 */
	Instance();

	/**
	 * @brief Instance class destructor.
	 */
	~Instance();
};

/**
 * @brief Template class for use with a ref-counted object, or any API-compliant class that extends Instance. Should be
 * used over a raw Instance, as it handles much of the referencing for the user.
 */
template <typename T>
class Ref {
	// Pointer to the class being referenced
	T *reference = nullptr;

	/**
	 * @brief General function to pass other Ref objects into to connect between.
	 * @param p_ref The reference to increment
	 */
	FORCE_INLINE void ref(const Ref &p_ref) {
		ref_ptr<false>(p_ref.reference);
	}

	/**
	 * @brief Increments the ref-count of a given pointer. If Init is true, then the ref-count is initialized, if not,
	 * then the ref-count is incremented on the pointer.
	 * @param p_ref The class pointer to make a reference towards
	 */
	template <bool Init>
	FORCE_INLINE void ref_ptr(T *p_ref) {
		if (reference == p_ref) {
			return;
		}

		Ref cleanup;
		cleanup.reference = reference;
		reference = p_ref;
		if (reference) {
			if (Init) {
				if (!reference->ref_init()) {
					reference = nullptr;
				}
			} else {
				if (!reference->reference()) {
					reference = nullptr;
				}
			}
		}
	}

public:
	FORCE_INLINE bool operator==(const Ref<T> &p_other) const {
		return reference == p_other.reference;
	}

	FORCE_INLINE bool operator!=(const Ref<T> &p_other) const {
		return reference != p_other.reference;
	}

	FORCE_INLINE T *operator*() const {
		return reference;
	}

	FORCE_INLINE T *operator->() const {
		return reference;
	}

	FORCE_INLINE T *ptr() const {
		return reference;
	}

	FORCE_INLINE void operator=(const Ref &p_other) {
		ref(p_other);
	}

	FORCE_INLINE void operator=(Ref &&p_from) {
		if (reference == p_from.reference) {
			return;
		}
		unref();
		reference = p_from.reference;
		p_from.reference = nullptr;
	}

	template <typename TOther>
	FORCE_INLINE void operator=(const Ref<TOther> &p_other) {
		ref_ptr<false>(Item::cast_to<T>(p_other.ptr()));
	}

	FORCE_INLINE void operator=(T *p_ref) {
		ref_ptr<true>(p_ref);
	}

	/**
	 * @brief Creates an instance of the given class, with a variadic number of arguments. Since Ref<T> objects don't
	 * instance the class on creation, this needs to be called in order to prevent any memory errors as the class is
	 * otherwise null.
	 * @param p_args... Variadic arguments to be used alongside the class.
	 */
	template <typename... VarArgs>
	void instantiate(VarArgs... p_args) {
		ref(vnew(T(p_args...)));
	}

	/**
	 * @brief Checks if the object holds a valid class.
	 * @returns True if yes, false if not.
	 */
	inline bool is_valid() const {
		return reference != nullptr;
	}

	/**
	 * @brief Checks if the object holds a null pointer, opposite of is_valid.
	 * @returns True if yes, false if not.
	 */
	inline bool is_null() const {
		return reference == nullptr;
	}

	/**
	 * @brief Decrements the refcount and sets the reference for the object to null. If the refcount = 0 on the object,
	 * delete it.
	 */
	void unref() {
		if (reference) {
			if (reference->unreference()) {
				vdelete(reference);
			}
			reference = nullptr;
		}
	}

	Ref(const Ref &p_from) {
		this->operator=(p_from);
	}

	Ref(Ref &&p_from) {
		reference = p_from.reference;
		p_from.reference = nullptr;
	}

	template <typename TOther>
	Ref(const Ref<TOther> &p_from) {
		this->operator=(p_from);
	}

	Ref(T *p_from) {
		this->operator=(p_from);
	}

	Ref() = default;

	~Ref() {
		unref();
	}
};
