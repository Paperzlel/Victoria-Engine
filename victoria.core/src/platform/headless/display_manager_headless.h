#pragma once

#include "core/os/display_manager.h"
#include "core/typedefs.h"

class DisplayManagerHeadless : public DisplayManager {
	Vector2i window_size;

public:
	static DisplayManager *create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error);

	virtual uint8_t create_window(const String &p_name,
								  uint16_t x,
								  uint16_t y,
								  uint16_t width,
								  uint16_t height,
								  WindowFlags p_flags = WINDOW_FLAG_DEFAULT) override;
	virtual void destroy_window(uint8_t p_id) override;

	virtual void set_use_vsync(bool p_value) override;

	virtual Vector2i get_window_size(uint8_t p_id) const override;
	virtual void set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) override;
	virtual void set_input_event_dispatch_callback(const CallableMethod &p_method, uint8_t p_id) override;

	virtual void toggle_mouse_mode(bool p_mode) override;
	virtual bool get_mouse_mode() const override;

	virtual void process_events() override;
	virtual void swap_buffers() override;

	virtual void finalize() override;

	DisplayManagerHeadless(const String &p_renderer, const Vector2i &p_size, Error *r_error);
	~DisplayManagerHeadless();
};
