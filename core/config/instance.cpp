#include "instance.h"

bool Instance::ref_init() {
    if (reference()) {
        if (!is_referenced() && init_rc.unref()) {
            unreference();
        }

        return true;
    } else {
        return false;
    }
}

bool Instance::reference() {
    bool can_ref = refcount.ref();

    return can_ref;
}

bool Instance::unreference() {
    bool can_die = refcount.unref();

    return can_die;
}

uint64 Instance::get_refcount() const {
    return refcount.get();
}

Instance::Instance() {
    refcount.init();
    init_rc.init();
}

Instance::~Instance() {

}