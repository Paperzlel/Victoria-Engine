#pragma once

#include <core/typedefs.h>
#include <core/data/hashtable.h>
#include <core/data/list.h>
#include <core/string/vstring.h>

#include "callable_method_pointer.h"

/**
 * @brief Core definitions for API-level classes.
 */
#define VREGISTER_CLASS(m_class, m_inherits)                                    \
private:                                                                        \
    friend class ClassRegistry;                                                 \
                                                                                \
public:                                                                         \
    virtual String get_class_name() const override {                            \
        return String(#m_class);                                                \
    }                                                                           \
                                                                                \
    virtual String get_inherited_class_name() const override {                  \
        return String(#m_inherits);                                             \
    }                                                                           \
                                                                                \
    static FORCE_INLINE String get_class_name_static() {                        \
        return String(#m_class);                                                \
    }                                                                           \
                                                                                \
    static FORCE_INLINE String get_inherited_class_name_static() {              \
        return String(#m_inherits);                                             \
    }                                                                           \
protected:                                                                      \
                                                                                \
    FORCE_INLINE void (Item::*_get_notification() const)(int) {                 \
        return (void(Item::*)(int)) & m_class::_notification;                   \
    }                                                                           \
                                                                                \
    FORCE_INLINE static void (*_get_bind_methods())() {                         \
        return &m_class::_bind_methods;                                         \
    }                                                                           \
public:                                                                         \
                                                                                \
    virtual void _notification_forwardv(int p_what) override {                  \
        m_inherits::_notification_forwardv(p_what);                             \
        if (m_class::_get_notification() != m_inherits::_get_notification()) {  \
            _notification(p_what);                                              \
        }                                                                       \
    }                                                                           \
                                                                                \
    virtual void _notification_backwardv(int p_what) override {                 \
        if (m_class::_get_notification() != m_inherits::_get_notification()) {  \
            _notification(p_what);                                              \
        }                                                                       \
        m_inherits::_notification_forwardv(p_what);                             \
    }                                                                           \
                                                                                \
    static void initialize_class() {                                            \
        static bool class_initialized = false;                                  \
        if (class_initialized) {                                                \
            return;                                                             \
        }                                                                       \
                                                                                \
        m_inherits::initialize_class();                                         \
        if (m_inherits::_get_bind_methods() != m_class::_get_bind_methods()) {  \
            _bind_methods();                                                    \
            class_initialized = true;                                           \
        }                                                                       \
    }                                                                           \
protected:                                                                      \
                                                                                \
    virtual void _initialize_classv() override {                                \
        initialize_class();                                                     \
    }                                                                           \
private:

enum Notification {
    NOTIFICATION_READY,
    NOTIFICATION_UPDATE,
    NOTIFICATION_PHYSICS_UPDATE,
    NOTIFICATION_QUIT,
    NOTIFICATION_PREDELETE,
    NOTIFICATION_ENTER_TREE,
    NOTIFICATION_CHILD_ENTERED_TREE,
    NOTIFICATION_CHILD_EXITED_TREE,
    NOTIFICATION_EXIT_TREE,
    NOTIFICATION_TRANSFORM_CHANGED,
    NOTIFICATION_VISIBILITY_CHANGED,
    NOTIFICATION_DRAW,
};


class VAPI Item {

    friend class ClassRegistry;

    HashTable<String, List<CallableMethod>> callables;

public:

    void _notification() {}

    virtual void _notification_forwardv(int p_what) {}
    virtual void _notification_backwardv(int p_what) {}

    friend bool predelete(Item *);
    bool _predelete();

    template <typename T>
    static T *cast_to(Item *p_object) {
        return p_object ? dynamic_cast<T *>(p_object) : nullptr;
    }

    template <typename T>
    static const T *cast_to(const Item *p_object) {
        return p_object ? dynamic_cast<const T *>(p_object) : nullptr;
    }

    void notification(int p_what, bool p_reversed = false);

    Error connect_method(const String &p_name, const CallableMethod &p_method);
    Error emit_methodp(const String &p_name, const Variant **p_args = nullptr, int p_argc = 0);

    template<typename... Args>
    FORCE_INLINE Error emit_method(const String &p_name, Args... p_args) {
        Variant args[sizeof...(p_args) + 1] = { p_args..., Variant() };
        const Variant *argptrs[sizeof...(p_args) + 1];

        for (int i = 0; i < sizeof...(p_args); i++) {
            argptrs[i] = &args[i];
        }

        return emit_methodp(p_name, argptrs, sizeof...(p_args));
    }

public:

    virtual String get_class_name() const {
        return "Item";
    }

    virtual String get_inherited_class_name() const {
        return String();
    }

    static String get_class_name_static() { return "Item"; }
    static String get_inherited_class_name_static() { return String(); }

protected:

    void _notification(int p_what) {}

    FORCE_INLINE void (Item::*_get_notification() const)(int) {
        return &Item::_notification;
    }

    FORCE_INLINE static void (*_get_bind_methods())() {
        return &Item::_bind_methods;
    }

    virtual void _initialize_classv() { initialize_class(); }

    static void _bind_methods() {}
public:

    static void initialize_class();

	Item();
	virtual ~Item();
};

VAPI bool predelete(Item *p_item);