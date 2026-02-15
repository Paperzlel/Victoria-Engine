#pragma once

#include "core/typedefs.h"
#include "core/string/vstring.h"

/**
 * @brief Core definitions for API-level classes.
 * @param m_class The class that is being defined
 * @param m_inherits The parent of the defined class
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

/**
 * @brief Every possible notification that could be sent. Notifications are a way of sending updates to a given Item and its respective children
 * whenever its state needs to be changed.
 */
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

/**
 * @brief Base class for all API-compliant classes. This class is not ref-counted, nor does it have a tree, so any systems that may be
 * unique from derived classes should use this one instead of other derived classes.
 */
class Item {

    friend class ClassRegistry;
public:

    /**
     * @brief Obtains the class name for the given class. Non-static, so classes that have been casted down will still display their highest
     * class.
     * @returns A String of the class name.
     */
    virtual String get_class_name() const {
        return "Item";
    }

    /**
     * @brief Obtains the inherited classes' name for the given class. Non-static, so classes that have been casted down will still display
     * their highest classes' parent.
     * @returns A String of the inherited classes' name.
     */
    virtual String get_inherited_class_name() const {
        return String();
    }

    /**
     * @brief Obtains the given classes' name. Static method for when the class name alone is needed.
     */
    static String get_class_name_static() { return "Item"; }
    
    /**
     * @brief Obtains the given classes' inherited class name. Static method for when the inherited class name alone is needed.
     */
    static String get_inherited_class_name_static() { return String(); }
protected:

    void _notification(int p_what) {}
    
    /**
     * @brief Obtains a function pointer to the classes' notification function. Notifications are different for every class and multiple levels
     * may define multiple functions, so this function is required in order to make the given calls.
     * @returns The function pointer to the notification function.
     */
    FORCE_INLINE void (Item::*_get_notification() const)(int) {
        return &Item::_notification;
    }
public:

    /**
     * @brief Virtual notification call function. Dispatches notifications to items. Do not call.
     * @param p_what The notification to dispatch
     */
    virtual void _notificationv(int p_what) {}

    /**
     * @brief Non-const casting function. Gives a general method for casting API-compliant classes up and down from their values.
     * @param p_object The instance of a class to cast
     * @returns The class casted depending on T.
     */
    template <typename T>
    static T *cast_to(Item *p_object) {
        return p_object ? dynamic_cast<T *>(p_object) : nullptr;
    }

    /**
     * @brief Const casting function. Gives a general method for casting API-compliant classes up and down from their initial classes.
     * @param p_object The instance of a class to cast
     * @returns Const version of the class casted depending on T.
     */
    template <typename T>
    static const T *cast_to(const Item *p_object) {
        return p_object ? dynamic_cast<const T *>(p_object) : nullptr;
    }

    /**
     * @brief Base notification function. Call on an Item or derived class whenever a notification needs to be sent up or down its derived classes.
     */
    void notification(int p_what);

    /**
     * @brief Item class constructor.
     */
	Item();
    /**
     * @brief Item class destructor. Virtual to allow for derived classes' destructors.
     */
	virtual ~Item();
};
