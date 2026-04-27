#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#	include "gl_manager_windows.h"
#	include "gl_manager_windows_angle.h"

#	include "core/os/display_manager.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <windowsx.h>

class DisplayManagerWindows : public DisplayManager {
private:
	HINSTANCE hInstance = nullptr;
	GLManagerWindows *gl_manager_windows = nullptr;
	EGLManagerANGLE *egl_manager_windows = nullptr;

	struct WindowData {
		uint8_t id;
		HWND hWnd;
		CallableMethod notification_callback;
		CallableMethod window_resize_callback;
		Vector2i position;
		Vector2i size;
	};

	bool first_frame = true;
	uint16_t old_x = 0;
	uint16_t old_y = 0;
	Vector2i centre;

	WindowData *window = nullptr;

public:
	static DisplayManager *create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error);
	static void register_windows_driver();

	LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual uint8_t create_window(const String &p_name,
								  uint16_t x,
								  uint16_t y,
								  uint16_t width,
								  uint16_t height,
								  WindowFlags p_flags = WINDOW_FLAG_DEFAULT) override;
	virtual void destroy_window(uint8_t p_id) override;

	virtual void set_use_vsync(bool p_value) override;

	virtual Vector2i get_window_rect() const override;
	virtual void set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) override;

	virtual void toggle_mouse_mode(bool p_mode) override;
	virtual bool get_mouse_mode() const override;

	virtual void process_events() override;
	virtual void swap_buffers() override;

	virtual void finalize() override;

	DisplayManagerWindows(const String &p_renderer, const Vector2i &p_size, Error *r_error);
	~DisplayManagerWindows();
};

#endif // PLATFORM_WINDOWS