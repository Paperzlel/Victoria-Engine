#include "core/io/input.h"

#include "core/error/error_macros.h"
#include "core/os/memory.h"

Input *Input::singleton = nullptr;

/**
 * @brief Returns the main instance of the Input class.
 */
Input *Input::get_singleton() {
	return singleton;
}

/**
 * @brief Checks if the given key is currently being held down by the user.
 */
bool Input::is_key_pressed(Key p_key) {
	return pressed_keys.has(p_key);
}

/**
 * @brief Checks if the given key has just began to be held down by the user. This event should only occur once, when
 * the user begins to hold down the key.
 */
bool Input::is_key_just_pressed(Key p_key) {
	return pressed_keys.has(p_key);
}

/**
 * @brief Checks if the given key is not being held down by the user.
 */
bool Input::is_key_released(Key p_key) {
	return !pressed_keys.has(p_key);
}

/**
 * @brief Checks if the given key has just stopped being held down by the user. This event should only occur once, when
 * the user begins to hold down the key.
 */
bool Input::is_key_just_released(Key p_key) {
	return !pressed_keys.has(p_key);
}

/**
 * @brief Checks if the given mouse button is currently being pressed down by the user.
 */
bool Input::is_mouse_button_pressed(MouseButton p_button) {
	ERR_FAIL_COND_MSG_R((int)p_button >= 4, "Mouse button given was too large.", false);
	return current_mouse.buttons[p_button] == true;
}

/**
 * @brief Checks if the given mouse button has just begun to be pressed down by the user. This event should only occur
 * once, when the user begins to hold down the button.
 */
bool Input::is_mouse_button_just_pressed(MouseButton p_button) {
	ERR_FAIL_COND_MSG_R((int)p_button >= 4, "Mouse button given was too large.", false);
	return current_mouse.buttons[p_button] == true && previous_mouse.buttons[p_button] == false;
}

/**
 * @brief Checks if the given mouse button is not being held down by the user.
 */
bool Input::is_mouse_button_released(MouseButton p_button) {
	ERR_FAIL_COND_MSG_R((int)p_button >= 4, "Mouse button given was too large.", false);
	return current_mouse.buttons[p_button] == false;
}

/**
 * @brief Checks if the given mouse button is not being held down by the user. This event should only occur once, when
 * the user begins to hold down the button.
 */
bool Input::is_mouse_button_just_released(MouseButton p_button) {
	ERR_FAIL_COND_MSG_R((int)p_button >= 4, "Mouse button given was too large.", false);
	return current_mouse.buttons[p_button] == false && previous_mouse.buttons[p_button] == true;
}

/**
 * @brief Returns the relative (the change in x/y position) position of the cursor on the screen. The general origin
 * for the given cursor is calculated as the centre of the screen, and any movements are then limited to whether the
 * cursor is captured or not.
 */
Vector2i Input::get_mouse_motion() {
	return relative_mouse_pos;
}

Vector2 Input::get_vector(Key p_left, Key p_right, Key p_up, Key p_down) {
	float x = is_key_pressed(p_left) && is_key_pressed(p_right)
				  ? 0
				  : (is_key_pressed(p_left) ? -1 : (is_key_pressed(p_right) ? 1 : 0));
	float y = is_key_pressed(p_down) && is_key_pressed(p_up)
				  ? 0
				  : (is_key_pressed(p_down) ? -1 : (is_key_pressed(p_up) ? 1 : 0));
	Vector2 ret = {x, y};
	return ret;
}

void Input::parse_input_event(const Ref<InputEvent> &p_event) {
	Ref<InputEventKey> key_event = p_event;
	if (key_event.is_valid()) {
		Key key = key_event->key;

		if ((int)key >= 256) {
			ERR_FAIL_MSG(vformat("Key code %i was too large", int(key)));
		}

		if (key_event->pressed) {
			if (!pressed_keys.has(key)) {
				pressed_keys.append(key);
			}
		} else {
			if (pressed_keys.has(key)) {
				int idx = pressed_keys.find(key);
				pressed_keys.remove_at(idx);
			}
		}
		// TODO: Re-add left/right command key options. Could be encoded in the uppermost bit of a key event.
	}

	Ref<InputEventMouseButton> mouse_button_event = p_event;

	if (mouse_button_event.is_valid()) {
		MouseButton mb = mouse_button_event->mb;
		if ((int)mb >= 4) {
			ERR_FAIL_MSG(vformat("Mouse button %i was too large", int(mb)));
		}

		current_mouse.buttons[mb] = mouse_button_event->pressed;
	}

	Ref<InputEventMouseMotion> mouse_motion_event = p_event;

	if (mouse_motion_event.is_valid()) {
		current_mouse.position = mouse_motion_event->absolute;
		relative_mouse_pos = mouse_motion_event->relative;
	}

	Ref<InputEventMouseScroll> mouse_scroll_event = p_event;

	if (mouse_scroll_event.is_valid()) {
		current_mouse.scroll = mouse_scroll_event->scroll;
	}
}

void Input::clear() {
	pressed_keys.clear();
	Memory::vzero(&current_mouse, sizeof(Mouse));
}

/**
 * @brief Updates the keyboards - it pushes the current keyboard and mouse from the current to the previous, so that
 * when the key processing has occured any events that rely on a difference in keypresses can be detected.
 */
void Input::update() {
	pressed_keys.clear();
	Memory::vcopy_memory(&previous_mouse, &current_mouse, sizeof(Mouse));
	relative_mouse_pos = Vector2i::zero();
}

/**
 * @brief Class constructor for the Input singleton. Sets all keypresses and button presses to false by default.
 */
Input::Input() {
	singleton = this;
	// Zero out the two keyboards and the mouse
	Memory::vzero(&current_mouse, sizeof(Mouse));
	Memory::vzero(&previous_mouse, sizeof(Mouse));
}