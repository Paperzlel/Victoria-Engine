#pragma once

#include "core/input/input_enums.h"
#include "core/input/keyboard.h"
#include "core/object/ref_counted.h"

/**
 * @brief Base event class. Handles all the logic related to how an input event is pressed and what that means for
 * overriding classes. Also provides a common set of variables for users to override.
 *
 */
class VCORE_API InputEvent : public RefCounted {
	VREGISTER_CLASS(InputEvent, RefCounted);

public:
	bool pressed = false; // Whether the given event is pressed down or not.

	/**
	 * @brief Checks to see if this class is equal to the given `InputEvent`, by comparing values. This function should
	 * be overridden by an implementing class as it cannot check if the classes are the same on its own.
	 * @param p_other The other class to check against.
	 * @return `true` if equal, and `false` if not. The non-overridden version always returns `false` as it can be
	 * assumed that they are not the same in this case.
	 */
	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const;

	InputEvent() {}
};

/**
 * @brief Key-based `InputEvent`. Emitted whenever the user presses a key.
 */
class VCORE_API InputEventKey : public InputEvent {
	VREGISTER_CLASS(InputEventKey, InputEvent);

public:
	Key key; // The key pressed/released by the user.

	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const override;

	InputEventKey() {}
};

/**
 * @brief Base class relating to all mouse events. Currently does nothing.
 *
 */
class VCORE_API InputEventMouse : public InputEvent {
	VREGISTER_CLASS(InputEventMouse, InputEvent);

public:
	InputEventMouse() {}
};

/**
 * @brief Mouse button related events. Emitted whenever the user presses a mouse button.
 *
 */
class VCORE_API InputEventMouseButton : public InputEventMouse {
	VREGISTER_CLASS(InputEventMouseButton, InputEventMouse);

public:
	InputEnums::MouseButton button; // The mouse button to check for.

	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const override;

	InputEventMouseButton() {}
};

/**
 * @brief Mouse movement related events. Emitted whenever the user moves the mouse.
 */
class VCORE_API InputEventMouseMotion : public InputEventMouse {
	VREGISTER_CLASS(InputEventMouseMotion, InputEventMouse);

public:
	Vector2i absolute; // The absolute position of the mouse on the window.
	Vector2i relative; // The relative movement of the mouse.

	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const override;

	InputEventMouseMotion() {}
};

/**
 * @brief Mouse scroll related events. Is capped at +-1 in either direction, or 0 for no scroll.
 */
class VCORE_API InputEventMouseScroll : public InputEventMouse {
	VREGISTER_CLASS(InputEventMouseScroll, InputEventMouse);

public:
	int8_t scroll; // The scroll of the mouse.

	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const override;

	InputEventMouseScroll() {}
};
