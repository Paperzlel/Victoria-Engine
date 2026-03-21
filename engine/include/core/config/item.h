#pragma once

#include "callable_method_pointer.h" // IWYU pragma: keep

#include "core/data/hashtable.h"
#include "core/data/list.h"
#include "core/string/vstring.h"
#include "core/typedefs.h"

/**
 * @brief Core definitions for API-level classes.
 * @param m_class The class that is being defined
 * @param m_inherits The parent of the defined class
 */
#define VREGISTER_CLASS(m_class, m_inherits)                                                                          \
private:                                                                                                              \
	friend class ClassRegistry;                                                                                       \
                                                                                                                      \
public:                                                                                                               \
	virtual String get_class_name() const override {                                                                  \
		return String(#m_class);                                                                                      \
	}                                                                                                                 \
                                                                                                                      \
	virtual String get_inherited_class_name() const override {                                                        \
		return String(#m_inherits);                                                                                   \
	}                                                                                                                 \
                                                                                                                      \
	static FORCE_INLINE String get_class_name_static() {                                                              \
		return String(#m_class);                                                                                      \
	}                                                                                                                 \
                                                                                                                      \
	static FORCE_INLINE String get_inherited_class_name_static() {                                                    \
		return String(#m_inherits);                                                                                   \
	}                                                                                                                 \
                                                                                                                      \
protected:                                                                                                            \
	FORCE_INLINE void (Item::*_get_notification() const)(int) {                                                       \
		return (void (Item::*)(int)) & m_class::_notification;                                                        \
	}                                                                                                                 \
                                                                                                                      \
	FORCE_INLINE static void (*_get_bind_methods())() {                                                               \
		return &m_class::_bind_methods;                                                                               \
	}                                                                                                                 \
                                                                                                                      \
public:                                                                                                               \
	virtual void _notification_forwardv(int p_what) override {                                                        \
		m_inherits::_notification_forwardv(p_what);                                                                   \
		if (m_class::_get_notification() != m_inherits::_get_notification()) {                                        \
			_notification(p_what);                                                                                    \
		}                                                                                                             \
	}                                                                                                                 \
                                                                                                                      \
	virtual void _notification_backwardv(int p_what) override {                                                       \
		if (m_class::_get_notification() != m_inherits::_get_notification()) {                                        \
			_notification(p_what);                                                                                    \
		}                                                                                                             \
		m_inherits::_notification_forwardv(p_what);                                                                   \
	}                                                                                                                 \
                                                                                                                      \
	static void initialize_class() {                                                                                  \
		static bool class_initialized = false;                                                                        \
		if (class_initialized) {                                                                                      \
			return;                                                                                                   \
		}                                                                                                             \
                                                                                                                      \
		m_inherits::initialize_class();                                                                               \
		if (m_inherits::_get_bind_methods() != m_class::_get_bind_methods()) {                                        \
			_bind_methods();                                                                                          \
			class_initialized = true;                                                                                 \
		}                                                                                                             \
	}                                                                                                                 \
                                                                                                                      \
protected:                                                                                                            \
	virtual void _initialize_classv() override {                                                                      \
		initialize_class();                                                                                           \
	}                                                                                                                 \
                                                                                                                      \
private:

/**
 * @brief Every possible notification that could be sent. Notifications are a way of sending updates to a given Item
 * and its respective children whenever its state needs to be changed.
 */
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

/**
 * @brief Base class for all API-compliant classes. This class is not ref-counted, nor does it have a tree, so any
 * systems that may be unique from derived classes should use this one instead of other derived classes.
 */
class VAPI Item {
	friend class ClassRegistry;

	HashTable<String, List<CallableMethod>> callables;

public:
	virtual void _notification_forwardv(int p_what) {}
	virtual void _notification_backwardv(int p_what) {}

	friend bool predelete(Item *);
	bool _predelete();

	/**
	 * @brief Non-const casting function. Gives a general method for casting API-compliant classes up and down from
	 * their values.
	 * @param p_object The instance of a class to cast
	 * @returns The class casted depending on T.
	 */
	template <typename T>
	static T *cast_to(Item *p_object) {
		return p_object ? dynamic_cast<T *>(p_object) : nullptr;
	}

	/**
	 * @brief Const casting function. Gives a general method for casting API-compliant classes up and down from their
	 * initial classes.
	 * @param p_object The instance of a class to cast
	 * @returns Const version of the class casted depending on T.
	 */
	template <typename T>
	static const T *cast_to(const Item *p_object) {
		return p_object ? dynamic_cast<const T *>(p_object) : nullptr;
	}

	/**
	 * @brief Base notification function. Call on an Item or derived class whenever a notification needs to be sent up
	 * or down its derived classes.
	 */
	void notification(int p_what, bool p_reversed = false);

	Error connect_method(const String &p_name, const CallableMethod &p_method);
	Error emit_methodp(const String &p_name, const Variant **p_args = nullptr, int p_argc = 0);

	template <typename... Args>
	FORCE_INLINE Error emit_method(const String &p_name, Args... p_args) {
		Variant args[sizeof...(p_args) + 1] = {p_args..., Variant()};
		const Variant *argptrs[sizeof...(p_args) + 1];

		for (u32 i = 0; i < sizeof...(p_args); i++) {
			argptrs[i] = &args[i];
		}

		return emit_methodp(p_name, argptrs, sizeof...(p_args));
	}

public:
	/**
	 * @brief Obtains the class name for the given class. Non-static, so classes that have been casted down will still
	 * display their highest class.
	 * @returns A String of the class name.
	 */
	virtual String get_class_name() const {
		return "Item";
	}

	/**
	 * @brief Obtains the inherited classes' name for the given class. Non-static, so classes that have been casted
	 * down will still display their highest classes' parent.
	 * @returns A String of the inherited classes' name.
	 */
	virtual String get_inherited_class_name() const {
		return String();
	}

	/**
	 * @brief Obtains the given classes' name. Static method for when the class name alone is needed.
	 */
	static String get_class_name_static() {
		return "Item";
	}

	/**
	 * @brief Obtains the given classes' inherited class name. Static method for when the inherited class name alone is
	 * needed.
	 */
	static String get_inherited_class_name_static() {
		return String();
	}

protected:
	/**
	 * @brief Virtual notification call function. Dispatches notifications to items. Do not call.
	 * @param p_what The notification to dispatch
	 */
	void _notification(int p_what) {}

	/**
	 * @brief Obtains a function pointer to the classes' notification function. Notifications are different for every
	 * class and multiple levels may define multiple functions, so this function is required in order to make the given
	 * calls.
	 * @returns The function pointer to the notification function.
	 */
	FORCE_INLINE void (Item::*_get_notification() const)(int) {
		return &Item::_notification;
	}

	FORCE_INLINE static void (*_get_bind_methods())() {
		return &Item::_bind_methods;
	}

	virtual void _initialize_classv() {
		initialize_class();
	}

	static void _bind_methods() {}

public:
	static void initialize_class();

	/**
	 * @brief Item class constructor.
	 */
	Item();

	/**
	 * @brief Item class destructor. Virtual to allow for derived classes' destructors.
	 */
	virtual ~Item();
};

VAPI bool predelete(Item *p_item);