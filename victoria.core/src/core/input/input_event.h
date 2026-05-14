#pragma once

#include "core/input/input_enums.h"
#include "core/input/keyboard.h"
#include "core/object/ref_counted.h"

class VAPI InputEvent : public RefCounted {
	VREGISTER_CLASS(InputEvent, RefCounted);

public:
	bool pressed = false;

	InputEvent() {}
};

class InputEventKey : public InputEvent {
	VREGISTER_CLASS(InputEventKey, InputEvent);

public:
	Key key;

	InputEventKey() {}
};

class InputEventMouse : public InputEvent {
	VREGISTER_CLASS(InputEventMouse, InputEvent);

public:
	InputEventMouse() {}
};

class InputEventMouseButton : public InputEventMouse {
	VREGISTER_CLASS(InputEventMouseButton, InputEventMouse);

public:
	MouseButton mb;

	InputEventMouseButton() {}
};

class InputEventMouseMotion : public InputEventMouse {
	VREGISTER_CLASS(InputEventMouseMotion, InputEventMouse);

public:
	Vector2i absolute;
	Vector2i relative;

	InputEventMouseMotion() {}
};

class InputEventMouseScroll : public InputEventMouse {
	VREGISTER_CLASS(InputEventMouseScroll, InputEventMouse);

public:
	int8_t scroll;

	InputEventMouseScroll() {}
};