#pragma once

#include "core/math/vector2i.h"
#include "core/object/ref_counted.h"
#include "core/typedefs.h"
#include "core/variant/variant_caster.h"

/**
 * @brief Class for every key type on a keyboard. This includes an entire ASCII character set, which some keys may be
 * bound to. Non-Latin keyboards
 *
 */
enum class Key {
	NONE = 0x00,

	SPECIAL = 1 << 22,

	CTRL = SPECIAL | 0x01,
	ALT = SPECIAL | 0x02,
	SHIFT = SPECIAL | 0x03,
	TAB = SPECIAL | 0x04,
	ESCAPE = SPECIAL | 0x05,
	CAPSLOCK = SPECIAL | 0x06,
	META = SPECIAL | 0x07,
	BACKSPACE = SPECIAL | 0x08,
	ENTER = SPECIAL | 0x09,

	F1 = SPECIAL | 0x0a,
	F2 = SPECIAL | 0x0b,
	F3 = SPECIAL | 0x0c,
	F4 = SPECIAL | 0x0d,
	F5 = SPECIAL | 0x0e,
	F6 = SPECIAL | 0x0f,
	F7 = SPECIAL | 0x10,
	F8 = SPECIAL | 0x11,
	F9 = SPECIAL | 0x12,
	F10 = SPECIAL | 0x13,
	F11 = SPECIAL | 0x14,
	F12 = SPECIAL | 0x15,
	F13 = SPECIAL | 0x16,
	F14 = SPECIAL | 0x17,
	F15 = SPECIAL | 0x18,
	F16 = SPECIAL | 0x19,
	F17 = SPECIAL | 0x1a,
	F18 = SPECIAL | 0x1b,
	F19 = SPECIAL | 0x1c,
	F20 = SPECIAL | 0x1d,
	F21 = SPECIAL | 0x1e,
	F22 = SPECIAL | 0x1f,
	F23 = SPECIAL | 0x21,
	F24 = SPECIAL | 0x22,

	UP = SPECIAL | 0x23,
	DOWN = SPECIAL | 0x24,
	LEFT = SPECIAL | 0x25,
	RIGHT = SPECIAL | 0x26,

	PAGEUP = SPECIAL | 0x21,
	PAGEDOWN = SPECIAL | 0x22,
	END = SPECIAL | 0x23,
	HOME = SPECIAL | 0x24,
	INSERT = SPECIAL | 0x25,
	KEY_DELETE = SPECIAL | 0x26,

	PRINTSCREEN = SPECIAL | 0x27,
	NUMLOCK = SPECIAL | 0x28,
	SCREENLOCK = SPECIAL | 0x29,
	VOLUMEMUTE = SPECIAL | 0x2a,
	VOLUMEDOWN = SPECIAL | 0x2b,
	VOLUMEUP = SPECIAL | 0x2c,
	PAUSE = SPECIAL | 0x2d,
	MENU = SPECIAL | 0x2e,
	CLEAR = SPECIAL | 0x2f,

	KP_0 = SPECIAL | 0x30,
	KP_1 = SPECIAL | 0x31,
	KP_2 = SPECIAL | 0x32,
	KP_3 = SPECIAL | 0x33,
	KP_4 = SPECIAL | 0x34,
	KP_5 = SPECIAL | 0x35,
	KP_6 = SPECIAL | 0x36,
	KP_7 = SPECIAL | 0x37,
	KP_8 = SPECIAL | 0x38,
	KP_9 = SPECIAL | 0x39,
	KP_MULTIPLY = SPECIAL | 0x3a,
	KP_ADD = SPECIAL | 0x3b,
	KP_SUBTRACT = SPECIAL | 0x3c,
	KP_DECIMAL = SPECIAL | 0x3d,
	KP_DIVIDE = SPECIAL | 0x3e,
	KP_ENTER = SPECIAL | 0x3f,
	KP_EQUAL = SPECIAL | 0x40,
	KP_COMMA = SPECIAL | 0x41,

	// Normal Latin-1 printable characters.

	SPACEBAR = 0x20,
	EXCLAMATION = 0x21,
	DOUBLE_QUOTE = 0x22,
	HASHTAG = 0x23,
	DOLLAR = 0x24,
	PERCENT = 0x25,
	AMPERSAND = 0x26,
	APOSTROPHE = 0x27,
	OPEN_PARENTHESIS = 0x28,
	CLOSE_PARENTHESIS = 0x29,
	ASTERISK = 0x2a,
	PLUS = 0x2b,
	COMMA = 0x2c,
	MINUS = 0x2d,
	PERIOD = 0x2e,
	SLASH = 0x2f,
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
	COLON = 0x3a,
	SEMICOLON = 0x3b,
	LESSTHAN = 0x3c,
	EQUALS = 0x3d,
	GREATERTHAN = 0x3e,
	QUESTIONMARK = 0x3f,
	AT = 0x40,
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
	OPENBRACKET = 0x5b,
	BACKSLASH = 0x5c,
	CLOSEDBRACKET = 0x5d,
	CARET = 0x5e,
	UNDERSCORE = 0x5f,
	BACKTICK = 0x60,
	OPENBRACE = 0x7b,
	BAR = 0x7c,
	CLOSEDBRACE = 0x7d,
	TILDE = 0x7e,
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
	Vector<Key> pressed_keys;

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
