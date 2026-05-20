#include "core/input/input_event.h"

bool InputEvent::action_is_equal(const Ref<InputEvent> &p_other) const {
	return false;
}

bool InputEventKey::action_is_equal(const Ref<InputEvent> &p_other) const {
	Ref<InputEventKey> key_event = p_other;
	if (key_event.is_null()) {
		return false;
	}

	if (key != key_event->key) {
		return false;
	}

	return true;
}

bool InputEventMouseButton::action_is_equal(const Ref<InputEvent> &p_other) const {
	Ref<InputEventMouseButton> btn_event = p_other;
	if (btn_event.is_null()) {
		return false;
	}

	if (button != btn_event->button) {
		return false;
	}

	return true;
}

bool InputEventMouseMotion::action_is_equal(const Ref<InputEvent> &p_other) const {
	Ref<InputEventMouseMotion> motion_event = p_other;
	if (motion_event.is_null()) {
		return false;
	}

	if (absolute != motion_event->absolute) {
		return false;
	}

	if (relative != motion_event->relative) {
		return false;
	}

	return true;
}

bool InputEventMouseScroll::action_is_equal(const Ref<InputEvent> &p_other) const {
	Ref<InputEventMouseScroll> scroll_event = p_other;
	if (scroll_event.is_null()) {
		return false;
	}

	if (scroll != scroll_event->scroll) {
		return false;
	}

	return true;
}
