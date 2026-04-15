#pragma once

#include "core/math/vector2i.h"
#include "core/object/ref_counted.h"
#include "core/typedefs.h"
#include "core/variant/variant_caster.h"

// Enum for every currently supported key. As of current, the given keycodes translate directly into the Windows API
// virtual keycodes, and as a result using a non-QWERTY keyboard is likely to produce some unintentional errors along
// the way.
enum Key {
	KEY_BACKSPACE = 0x08,
	KEY_TAB = 0x09,
	KEY_CLEAR = 0x0C,
	KEY_ENTER = 0x0D,
	KEY_SHIFT = 0x10,
	KEY_CTRL = 0x11,
	KEY_ALT = 0x12,
	KEY_CAPSLOCK = 0x14,
	KEY_ESCAPE = 0x1B,
	KEY_SPACEBAR = 0x20,
	KEY_PAGEUP = 0x21,
	KEY_PAGEDOWN = 0x22,
	KEY_END = 0x23,
	KEY_HOME = 0x24,
	KEY_LEFT = 0x25,
	KEY_UP = 0x26,
	KEY_RIGHT = 0x27,
	KEY_DOWN = 0x28,
	KEY_INSERT = 0x2D,
	KEY_DELETE = 0x2E,
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
	KEY_A = 0x41,
	KEY_B = 0x42,
	KEY_C = 0x43,
	KEY_D = 0x44,
	KEY_E = 0x45,
	KEY_F = 0x46,
	KEY_G = 0x47,
	KEY_H = 0x48,
	KEY_I = 0x49,
	KEY_J = 0x4A,
	KEY_K = 0x4B,
	KEY_L = 0x4C,
	KEY_M = 0x4D,
	KEY_N = 0x4E,
	KEY_O = 0x4F,
	KEY_P = 0x50,
	KEY_Q = 0x51,
	KEY_R = 0x52,
	KEY_S = 0x53,
	KEY_T = 0x54,
	KEY_U = 0x55,
	KEY_V = 0x56,
	KEY_W = 0x57,
	KEY_X = 0x58,
	KEY_Y = 0x59,
	KEY_Z = 0x5A,
	KEY_NUMPAD0 = 0x60,
	KEY_NUMPAD1 = 0x61,
	KEY_NUMPAD2 = 0x62,
	KEY_NUMPAD3 = 0x63,
	KEY_NUMPAD4 = 0x64,
	KEY_NUMPAD5 = 0x65,
	KEY_NUMPAD6 = 0x66,
	KEY_NUMPAD7 = 0x67,
	KEY_NUMPAD8 = 0x68,
	KEY_NUMPAD9 = 0x69,
	KEY_NUMPADMULTIPLY = 0x6A,
	KEY_NUMPADADD = 0x6B,
	KEY_NUMPADSUBTRACT = 0x6D,
	KEY_NUMPADDECIMAL = 0x6E,
	KEY_NUMPADDIVIDE = 0x6F,
	KEY_F1 = 0x70,
	KEY_F2 = 0x71,
	KEY_F3 = 0x72,
	KEY_F4 = 0x73,
	KEY_F5 = 0x74,
	KEY_F6 = 0x75,
	KEY_F7 = 0x76,
	KEY_F8 = 0x77,
	KEY_F9 = 0x78,
	KEY_F10 = 0x79,
	KEY_F11 = 0x7A,
	KEY_F12 = 0x7B,
	KEY_F13 = 0x7C,
	KEY_F14 = 0x7D,
	KEY_F15 = 0x7E,
	KEY_F16 = 0x7F,
	KEY_F17 = 0x80,
	KEY_F18 = 0x81,
	KEY_F19 = 0x82,
	KEY_F20 = 0x83,
	KEY_F21 = 0x84,
	KEY_F22 = 0x85,
	KEY_F23 = 0x86,
	KEY_F24 = 0x87,
	// WARNING: These keys currently don't work as intended yet, at least on Windows for the meanwhile. Do not use
	KEY_LSHIFT = 0xA0,
	KEY_RSHIFT = 0xA1,
	KEY_LCTRL = 0xA2,
	KEY_RCTRL = 0xA3,
	KEY_LALT = 0xA4,
	KEY_RALT = 0xA5,
	KEY_PLUS = 0xBB,
	KEY_COMMA = 0xBC,
	KEY_MINUS = 0xBD,
	KEY_PERIOD = 0xBE,

	// TODO: Add the other keys (namely the OEM keys, the window key, etc.)

	KEY_MAX_KEYS
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
	uint8_t scroll;

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
	uint8_t get_mouse_scroll() const {
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
