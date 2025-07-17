#pragma once

#include "core/typedefs.h"
#include "core/data/atomic_counter.h"
#include "core/os/os.h"
#include "core/string/vstring.h"
#include "item.h"

class Instance : public Item {
    VREGISTER_CLASS(Instance, Item)

    Refcount refcount;
    Refcount init_rc;
public:

    // references

    bool ref_init();
    bool reference();
    bool unreference();
    uint64 get_refcount() const;
    bool is_referenced() const { return init_rc.get() != 1; }

    Instance();
    ~Instance();
};


template <typename T>
class Ref {
    T *reference = nullptr;

    FORCE_INLINE void ref(const Ref &p_ref) { 
        ref_ptr<false>(p_ref.reference); 
    }

    template<bool Init>
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

    template <typename... VarArgs>
    void instantiate(VarArgs... p_args) {
        ref(new T(p_args...));
    }

    inline bool is_valid() const { return reference != nullptr; }
    inline bool is_null() const { return reference == nullptr; }

    void unref() {
        if (reference) {
            if (reference->unreference()) {
                delete reference;
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
