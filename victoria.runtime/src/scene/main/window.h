#pragma once

#include "scene/main/viewport.h"

#include <core/os/display_manager.h>

class VAPI Window : public Viewport {
	VREGISTER_CLASS(Window, Viewport)

	uint8_t window_id = DisplayManager::INVALID_WINDOW_ID;

	void _window_resize_callback(uint8_t p_window_id);

protected:
	void _notification(int p_what);

public:
	Window();
};
