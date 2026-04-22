#pragma once

#include "core/math/vector2i.h"
#include "core/object/ref_counted.h"
#include "core/typedefs.h"
#include "core/variant/variant_caster.h"

// Enum for every currently supported key. As of current, the given keycodes translate directly into the Windows API
// virtual keycodes, and as a result using a non-QWERTY keyboard is likely to produce some unintentional errors along
// the way.
enum class Key : int {
	NONE = 0x00,
	BACKSPACE = 0x08,
	TAB = 0x09,
	CLEAR = 0x0C,
	ENTER = 0x0D,
	SHIFT = 0x10,
	CTRL = 0x11,
	ALT = 0x12,
	CAPSLOCK = 0x14,
	ESCAPE = 0x1B,
	SPACEBAR = 0x20,
	PAGEUP = 0x21,
	PAGEDOWN = 0x22,
	END = 0x23,
	HOME = 0x24,
	LEFT = 0x25,
	UP = 0x26,
	RIGHT = 0x27,
	DOWN = 0x28,
	INSERT = 0x2D,
	DELETE = 0x2E,
	KEY_0 = 0x30,
	KEY_1 = 0x31,
	KEY_2 = 0x32,
	KEY_3 = 0x33,
	KEY_4 = 0x34,
	KEY_5 = 0x35,
	KEY_6 = 0x36,
	KEY_7 = 0x37,
	KEY_8 = 0x38,
	KEY_9 = 0x39,
	A = 0x41,
	B = 0x42,
	C = 0x43,
	D = 0x44,
	E = 0x45,
	F = 0x46,
	G = 0x47,
	H = 0x48,
	I = 0x49,
	J = 0x4A,
	K = 0x4B,
	L = 0x4C,
	M = 0x4D,
	N = 0x4E,
	O = 0x4F,
	P = 0x50,
	Q = 0x51,
	R = 0x52,
	S = 0x53,
	T = 0x54,
	U = 0x55,
	V = 0x56,
	W = 0x57,
	X = 0x58,
	Y = 0x59,
	Z = 0x5A,
	NUMPAD0 = 0x60,
	NUMPAD1 = 0x61,
	NUMPAD2 = 0x62,
	NUMPAD3 = 0x63,
	NUMPAD4 = 0x64,
	NUMPAD5 = 0x65,
	NUMPAD6 = 0x66,
	NUMPAD7 = 0x67,
	NUMPAD8 = 0x68,
	NUMPAD9 = 0x69,
	NUMPADMULTIPLY = 0x6A,
	NUMPADADD = 0x6B,
	NUMPADSUBTRACT = 0x6D,
	NUMPADDECIMAL = 0x6E,
	NUMPADDIVIDE = 0x6F,
	F1 = 0x70,
	F2 = 0x71,
	F3 = 0x72,
	F4 = 0x73,
	F5 = 0x74,
	F6 = 0x75,
	F7 = 0x76,
	F8 = 0x77,
	F9 = 0x78,
	F10 = 0x79,
	F11 = 0x7A,
	F12 = 0x7B,
	F13 = 0x7C,
	F14 = 0x7D,
	F15 = 0x7E,
	F16 = 0x7F,
	F17 = 0x80,
	F18 = 0x81,
	F19 = 0x82,
	F20 = 0x83,
	F21 = 0x84,
	F22 = 0x85,
	F23 = 0x86,
	F24 = 0x87,
	// WARNING: These keys currently don't work as intended yet, at least on Windows for the meanwhile. Do not use
	LSHIFT = 0xA0,
	RSHIFT = 0xA1,
	LCTRL = 0xA2,
	RCTRL = 0xA3,
	LALT = 0xA4,
	RALT = 0xA5,
	PLUS = 0xBB,
	COMMA = 0xBC,
	MINUS = 0xBD,
	PERIOD = 0xBE,

	OPEN_BRACKET = 0xBF,
	CLOSED_BRACKET = 0xC0,

	SEMICOLON = 0xC1,
	APOSTROPHE = 0xC2,
	BACKTICK = 0xC3,
	BACKSLASH = 0xC4,
	SLASH = 0xC5,
	NUMLOCK = 0xC6,
	SCREENLOCK = 0xC7,
	EUQUOTELEFT = 0xC8,
	NUMPADENTER = 0xC9,
	PRINTSCREEN = 0xCA,
	VOLUMEMUTE = 0xCB,
	VOLUMEDOWN = 0xCC,
	VOLUMEUP = 0xCD,
	NUMPADEQUAL = 0xCE,
	NUMPADCOMMA = 0xCF,
	PAUSE = 0xD0,
	META = 0xD1,
	MENU = 0xD2,

	// TODO: Add the other keys (namely the OEM keys, the window key, etc.)

	MAX_KEYS
};

// Enum for each mouse button that the current API allows for calls to be made to
enum MouseButton {
	MOUSE_LBUTTON,
	MOUSE_RBUTTON,
	MOUSE_MBUTTON,

	MOUSE_MAX
};

/**
 * @brief Structure that represents the current keyboard layout. Each key is represented as a boolean for whether it is
 * pressed down or not.
 */
struct Keyboard {
public:
	bool keys[256];
};

/**
 * @brief Structure that represents the current mouse wheel layout. Contains all the mouse buttons, as well as the
 * scroll amount (-1 or 1) and the position of the cursor as a Vector2i.
 */
struct Mouse {
public:
	bool buttons[3];
	uint8_t scroll;
	Vector2i position;
};

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

/**
 * @brief Input singleton class.
 */
class VAPI Input {
	Keyboard current_keyboard;
	Keyboard previous_keyboard;

	Mouse current_mouse;
	Mouse previous_mouse;

	Vector2i relative_mouse_pos;

	static Input *singleton;

public:
	static Input *get_singleton();

	// Key presses/releases

	bool is_key_pressed(Key p_key);
	bool is_key_just_pressed(Key p_key);
	bool is_key_released(Key p_key);
	bool is_key_just_released(Key p_key);

	// Mouse button presses/releases

	bool is_mouse_button_pressed(MouseButton p_button);
	bool is_mouse_button_just_pressed(MouseButton p_button);
	bool is_mouse_button_released(MouseButton p_button);
	bool is_mouse_button_just_released(MouseButton p_button);

	// Mouse wheel movements

	/**
	 * @brief Gets the scroll direction of the mouse wheel. -1 is downwards, 0 is netural, and 1 is upwards.
	 */
	int8_t get_mouse_scroll() const {
		return current_mouse.scroll;
	}

	// Mouse movement commands

	/**
	 * @brief Gets the current position of the mouse in absolute screen coordinates, with (0, 0) being in the
	 * top-leftmost corner in the Windows implementation.
	 */
	Vector2i get_mouse_screen_position() const {
		return current_mouse.position;
	}
	Vector2i get_mouse_motion();

	Vector2 get_vector(Key p_left, Key p_right, Key p_up, Key p_down);

	// Event processing

	void parse_input_event(const Ref<InputEvent> &p_event);
	void clear();

	void update();
	Input();
};

VARIANT_CAST_ENUM(Key);
VARIANT_CAST_ENUM(MouseButton);
