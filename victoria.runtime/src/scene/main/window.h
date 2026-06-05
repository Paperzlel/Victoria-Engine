#pragma once

#include "scene/main/viewport.h"

#include <core/object/ref_counted.h>
#include <core/os/display_manager.h>

class InputEvent;

class VRUNTIME_API Window : public Viewport {
	VREGISTER_CLASS(Window, Viewport);

	uint8_t window_id = DisplayManager::INVALID_WINDOW_ID;

	void _window_resize_callback(uint8_t p_window_id);
	void _window_input_callback(const Ref<InputEvent> &p_event);

protected:
	void _notification(int p_what);

public:
	Window();
};
