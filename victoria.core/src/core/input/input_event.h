#pragma once

#include "core/input/input_enums.h"
#include "core/input/keyboard.h"
#include "core/object/ref_counted.h"

class VAPI InputEvent : public RefCounted {
	VREGISTER_CLASS(InputEvent, RefCounted);

public:
	bool pressed = false;

	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const;

	InputEvent() {}
};

class VAPI InputEventKey : public InputEvent {
	VREGISTER_CLASS(InputEventKey, InputEvent);

public:
	Key key;

	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const override;

	InputEventKey() {}
};

class VAPI InputEventMouse : public InputEvent {
	VREGISTER_CLASS(InputEventMouse, InputEvent);

public:
	InputEventMouse() {}
};

class VAPI InputEventMouseButton : public InputEventMouse {
	VREGISTER_CLASS(InputEventMouseButton, InputEventMouse);

public:
	InputEnums::MouseButton button;

	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const override;

	InputEventMouseButton() {}
};

class VAPI InputEventMouseMotion : public InputEventMouse {
	VREGISTER_CLASS(InputEventMouseMotion, InputEventMouse);

public:
	Vector2i absolute;
	Vector2i relative;

	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const override;

	InputEventMouseMotion() {}
};

class VAPI InputEventMouseScroll : public InputEventMouse {
	VREGISTER_CLASS(InputEventMouseScroll, InputEventMouse);

public:
	int8_t scroll;

	virtual bool action_is_equal(const Ref<InputEvent> &p_other) const override;

	InputEventMouseScroll() {}
};