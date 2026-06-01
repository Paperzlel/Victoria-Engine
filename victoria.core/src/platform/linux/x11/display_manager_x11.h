#pragma once

#include "core/typedefs.h"
#if defined(PLATFORM_LINUX) && defined(X11_ENABLED)

#	include "gl_manager_egl_x11.h"
#	include "gl_manager_x11.h"

#	include "core/os/display_manager.h"
#	include "core/object/ref_counted.h"

#	include <X11/Xlib.h>

class InputEvent;

/**
 * @brief Extension of the `DisplayManager` class for the X11 API.
 */
class DisplayManagerX11 : public DisplayManager {
private:
	// The current connection to the X11 server
	Display *display = nullptr;
	// The screen that the X11 projects onto
	Screen *screen = nullptr;
	// The screen number that X11 wants to project onto
	int screen_id = 0;

	// Our current instance of the OpenGL context manager
	GLManagerX11 *gl_manager_x11 = nullptr;
	// Our current instance of the X11 EGL context manager
	EGLManagerX11 *egl_manager_x11 = nullptr;

	/**
	 * @brief A struct representing all the data we want a single window to handle.
	 */
	struct WindowData {
		// The current window's ID
		uint8_t id;
		// The X11 window this given window links to
		Window win;
		// The window's attributes we want to store and edit
		XSetWindowAttributes window_attribs;
		// Atom that refers to the state of the window
		Atom wm_state_atom;
		// The atom that refers to the closing of the window
		Atom wm_close_atom;

		CallableMethod window_resize_callback;

		// Callable result for an input event
		CallableMethod input_event_callback;

		// Position of the window
		Vector2i position;

		// Size of the window
		Vector2i size;

		bool minimized = false;
		bool maximised = false;
	};

	bool first_frame = true;
	uint16_t old_x = 0;
	uint16_t old_y = 0;
	Vector2i centre;

	// The current window being used by the application
	WindowData *window = nullptr;

	void _update_window(XEvent *p_event);

	void _update_wm_properties();

	static void _dispatch_input_event_s(const Ref<InputEvent> &p_event);
	void _dispatch_input_event(const Ref<InputEvent> &p_event);

public:
	static DisplayManager *create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error);
	static void register_x11_driver();

	/**
	 * @brief Returns a pointer to the current display server being used. Due to our goal of self-containment, this
	 * function should not be usable outside of the X11 interface to avoid compiler errors.
	 * @returns A pointer to the current X11 display being used.
	 */
	Display *get_display() const {
		return display;
	}
	/**
	 * @brief Gets a pointer to the current screen the X11 interface is rendering to. Should be used only within any
	 * X11-related code to prevent any compiler issues.
	 * @returns A pointer to the current screen X11 is rendering to.
	 */
	Screen *get_screen() const {
		return screen;
	}
	/**
	 * @brief Gets the current screen ID for the X11 screen we are using.
	 * @returns The current screen ID for the X11 server.
	 */
	int get_screen_id() const {
		return screen_id;
	}

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

	DisplayManagerX11(const String &p_renderer, const Vector2i &p_size, Error *r_error);
	~DisplayManagerX11();
};

#endif // defined(PLATFORM_LINUX) && defined(X11_ENABLED)
