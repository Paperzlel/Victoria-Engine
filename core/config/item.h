#pragma once

#include "core/typedefs.h"
#include "core/string/vstring.h"

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
                                                                                \
protected:                                                                      \
    FORCE_INLINE void (Item::*_get_notification() const)(int) {                 \
        return (void(Item::*)(int)) & m_class::_notification;                   \
    }                                                                           \
                                                                                \
public:                                                                         \
    virtual void _notificationv(int p_what) override {                          \
        m_inherits::_notificationv(p_what);                                     \
        if (m_class::_get_notification() != m_inherits::_get_notification()) {  \
            _notification(p_what);                                              \
        }                                                                       \
    }                                                                           \
                                                                                \
private:

enum Notification {
    NOTIFICATION_READY,
    NOTIFICATION_UPDATE,
    NOTIFICATION_PHYSICS_UPDATE,
    NOTIFICATION_QUIT,
    NOTIFICATION_PREDELETE,
    NOTIFICATION_ENTER_TREE,
    NOTIFICATION_EXIT_TREE,
    NOTIFICATION_TRANSFORM_CHANGED,
    NOTIFICATION_VISIBILITY_CHANGED,
};

class Item {

    friend class ClassRegistry;
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
public:

    virtual void _notificationv(int p_what) {}

    template <typename T>
    static T *cast_to(Item *p_object) {
        return p_object ? dynamic_cast<T *>(p_object) : nullptr;
    }

    template <typename T>
    static const T *cast_to(const Item *p_object) {
        return p_object ? dynamic_cast<const T *>(p_object) : nullptr;
    }

    void notification(int p_what);

	Item();
	virtual ~Item();
};
