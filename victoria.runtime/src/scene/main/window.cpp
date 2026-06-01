#include "scene/main/window.h"

#include "rendering/rendering_manager.h"

#include <core/input/input_event.h>
#include <core/object/callable_method_pointer.h>

void Window::_window_resize_callback(uint8_t p_window_id) {
	ERR_FAIL_COND_MSG(window_id != p_window_id, "Scene window ID was not the same as DisplayManager window ID.");

	set_viewport_size(DisplayManager::get_singleton()->get_window_size(window_id));
}

void Window::_window_input_callback(const Ref<InputEvent> &p_event) {
	if (!is_inside_tree()) {
		return;
	}
}

void Window::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (!get_parent()) {
				// Is root window
				window_id = 0;
				set_viewport_size(DisplayManager::get_singleton()->get_window_size(window_id));
				RM::get_singleton()->viewport_set_window(get_viewport_rid(), window_id);

				DisplayManager::get_singleton()->set_window_resize_callback(
					callable_mp(this, &Window::_window_resize_callback),
					window_id);

				DisplayManager::get_singleton()->set_input_event_dispatch_callback(
					callable_mp(this, &Window::_window_input_callback),
					window_id);
			}
		} break;
	}
}

Window::Window() {}
