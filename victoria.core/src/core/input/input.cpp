#include "core/input/input.h"

#include "core/error/error_macros.h"
#include "core/input/input_map.h"
#include "core/string/print_string.h"

Input *Input::singleton = nullptr;

Input *Input::get_singleton() {
	return singleton;
}

uint8_t Input::_get_mask_from_button(InputEnums::MouseButton p_button) {
	switch (p_button) {
		case InputEnums::MOUSE_LBUTTON:
			return MBM_LEFT;
		case InputEnums::MOUSE_RBUTTON:
			return MBM_RIGHT;
		case InputEnums::MOUSE_MBUTTON:
			return MBM_MIDDLE;
		default:
			return 0;
	}
}

bool Input::is_action_pressed(const VName &p_action_name) {
	ERR_FAIL_COND_MSG_R(!InputMap::get_singleton()->has_action(p_action_name),
						vformat("Input action \'%s\' does not exist.", p_action_name.get_string().get_data()),
						false);

	HashTable<VName, ActionCache>::ConstIterator E = action_cache.find(p_action_name);
	if (!E) {
		return false;
	}

	return E->value.pressed;
}

bool Input::is_action_released(const VName &p_action_name) {
	ERR_FAIL_COND_MSG_R(!InputMap::get_singleton()->has_action(p_action_name),
						vformat("Input action \'%s\' does not exist.", p_action_name.get_string().get_data()),
						true);

	HashTable<VName, ActionCache>::ConstIterator E = action_cache.find(p_action_name);
	if (!E) {
		return true;
	}

	return E->value.pressed;
}

bool Input::is_action_just_pressed(const VName &p_action_name) {
	ERR_FAIL_COND_MSG_R(!InputMap::get_singleton()->has_action(p_action_name),
						vformat("Input action \'%s\' does not exist.", p_action_name.get_string().get_data()),
						false);

	HashTable<VName, ActionCache>::ConstIterator E = action_cache.find(p_action_name);
	if (!E) {
		return false;
	}

	// TODO: Add engine frame updates
	return E->value.pressed && E->value.engine_frame == 1;
}

bool Input::is_action_just_released(const VName &p_action_name) {
	ERR_FAIL_COND_MSG_R(!InputMap::get_singleton()->has_action(p_action_name),
						vformat("Input action \'%s\' does not exist.", p_action_name.get_string().get_data()),
						true);

	HashTable<VName, ActionCache>::ConstIterator E = action_cache.find(p_action_name);
	if (!E) {
		return true;
	}

	// TODO: Add engine frame updates
	return E->value.pressed && E->value.engine_frame == 1;
}

bool Input::is_key_pressed(Key p_key) {
	return pressed_keys.find(p_key) != nullptr;
}

bool Input::is_key_released(Key p_key) {
	return pressed_keys.find(p_key) == nullptr;
}

bool Input::is_mouse_button_pressed(InputEnums::MouseButton p_button) {
	ERR_FAIL_COND_MSG_R((int)p_button >= 4, "Mouse button given was too large.", false);
	return mouse_info.button_mask & _get_mask_from_button(p_button);
}

bool Input::is_mouse_button_released(InputEnums::MouseButton p_button) {
	ERR_FAIL_COND_MSG_R((int)p_button >= 4, "Mouse button given was too large.", false);
	return (mouse_info.button_mask & _get_mask_from_button(p_button)) == 0;
}

Vector2i Input::get_mouse_motion() {
	return mouse_info.relative_motion;
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

		RBSet<Key>::Element *e = pressed_keys.find(key);
		if (key_event->pressed && e == nullptr) {
			pressed_keys.insert(key);
		} else if (e != nullptr) {
			print_line("Erased key", (int)key);
			pressed_keys.erase(key);
		}
		// TODO: Re-add left/right command key options. Could be encoded in the uppermost bit of a key event.
	}

	Ref<InputEventMouseButton> mouse_button_event = p_event;

	if (mouse_button_event.is_valid()) {
		InputEnums::MouseButton mb = mouse_button_event->button;
		if ((int)mb >= 4) {
			ERR_FAIL_MSG(vformat("Mouse button %i was too large", int(mb)));
		}

		uint8_t mask = _get_mask_from_button(mouse_button_event->button);
		if (mouse_button_event->pressed) {
			mouse_info.button_mask |= mask;
		} else {
			mouse_info.button_mask &= ~mask;
		}
	}

	Ref<InputEventMouseMotion> mouse_motion_event = p_event;

	if (mouse_motion_event.is_valid()) {
		mouse_info.position = mouse_motion_event->absolute;
		mouse_info.relative_motion = mouse_motion_event->relative;
	}

	Ref<InputEventMouseScroll> mouse_scroll_event = p_event;

	if (mouse_scroll_event.is_valid()) {
		mouse_info.scroll = mouse_scroll_event->scroll;
	}

	for (const VName &item : InputMap::get_singleton()->get_actions_for_event(p_event)) {
		ActionCache &cache_item = action_cache[item];
		cache_item.pressed = p_event->pressed;
		cache_item.engine_frame = 1; // TODO: Engine frames
	}
}

void Input::clear() {
	pressed_keys.clear();
	for (KeyValue<VName, ActionCache> &kv : action_cache) {
		kv.value.pressed = false;
	}
}

void Input::update() {
	mouse_info.relative_motion = Vector2i::zero();
}

/**
 * @brief Class constructor for the Input singleton. Sets all keypresses and button presses to false by default.
 */
Input::Input() {
	singleton = this;
}

Input::~Input() {
	singleton = nullptr;
}
