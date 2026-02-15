#include <core/config/class_registry.h>

HashTable<String, ClassRegistry::ClassInfo> ClassRegistry::classes;

Item *ClassRegistry::instantiate(const String &p_class) {
    ClassInfo *ci;

    ci = classes.get_ptr(p_class);
    ERR_COND_NULL_MSG_R(ci, "Could not get the given class as a ClassInfo struct. Please register the given class with the ClassRegistry.", nullptr);
    ERR_FAIL_COND_MSG_R(ci->is_registered == false, "Cannot instance a class that has not been registered to the ClassRegistry.", nullptr);
    return ci->creation_func();
}

void ClassRegistry::add_signal(const String &p_class, const String &p_signal) {
    ClassInfo *c = classes.get_ptr(p_class);

    if (c->signals.has(p_signal)) {
        return;
    }

    c->signals.push_back(p_signal);
}

bool ClassRegistry::has_signal(const String &p_class, const String &p_signal) {
    ClassInfo *c = classes.get_ptr(p_class);
    while (c) {
        if (c->signals.has(p_signal)) {
            return true;
        }

        c = classes.get_ptr(c->inherits);
    }

    return false;
}