#pragma once

#include "viewport.h"

#include "core/os/display_manager.h"
#include "core/typedefs.h"

class VAPI Window : public Viewport {
	VREGISTER_CLASS(Window, Viewport)

	u8 window_id = DisplayManager::INVALID_WINDOW_ID;

	void _window_resize_callback(u8 p_window_id);

protected:
	void _notification(int p_what);

public:
	Window();
};
