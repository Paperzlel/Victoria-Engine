#include "display_manager_headless.h"

uint8_t DisplayManagerHeadless::create_window(const String &p_name,
											  uint16_t x,
											  uint16_t y,
											  uint16_t width,
											  uint16_t height,
											  WindowFlags p_flags) {
	window_size = Vector2i(width, height);
	return 0;
}

void DisplayManagerHeadless::destroy_window(uint8_t p_id) {
	ERR_FAIL_COND_MSG(p_id != 0, "Invalid window ID on a headless server!");
}

void DisplayManagerHeadless::set_use_vsync(bool p_value) {}

Vector2i DisplayManagerHeadless::get_window_size(uint8_t p_id) const {
	ERR_FAIL_COND_R(p_id != 0, Vector2i());
	return window_size;
}

void DisplayManagerHeadless::set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) {}

void DisplayManagerHeadless::set_input_event_dispatch_callback(const CallableMethod &p_method, uint8_t p_id) {}

void DisplayManagerHeadless::toggle_mouse_mode(bool p_mode) {}

bool DisplayManagerHeadless::get_mouse_mode() const {
	return false;
}

void DisplayManagerHeadless::process_events() {}

void DisplayManagerHeadless::swap_buffers() {}

void DisplayManagerHeadless::finalize() {}

DisplayManager *DisplayManagerHeadless::create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	return vnew(DisplayManagerHeadless(p_renderer, p_size, r_error));
}

DisplayManagerHeadless::DisplayManagerHeadless(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	uint8_t id = create_window("Victoria Engine Window", 0, 0, p_size.x, p_size.y);
	if (r_error) {
		if (id != 0) {
			*r_error = ERR_CANT_CREATE;
		} else {
			*r_error = OK;
		}
	}
}

DisplayManagerHeadless::~DisplayManagerHeadless() {}