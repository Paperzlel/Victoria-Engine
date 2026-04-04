#pragma once

#include "core/typedefs.h"
#if PLATFORM_LINUX

#	include <X11/Xlib.h>
#	include <glad/gl.h>
#	include <glad/glx.h>

/**
 * @brief The current OpenGL context and window manager. Controls the creation, changing, and deletion of contexts, as
 * well as the viewport size and the current framebuffer.
 */
class GLManagerX11 {
private:
	/**
	 * @brief Struct for all the information held by an OpenGL window.
	 */
	struct GLWindow {
		// The current ID of the window
		uint8_t id;

		// A pointer to the X11 window struct
		::Window *win;
		// The current GLX context that the window has
		GLXContext glx_context;
	};

	// The current display connection to X11
	Display *display = nullptr;
	// The current screen ID for the X11 screen
	int screen_id = 0;
	// The framebuffer config the application is using
	GLXFBConfig fb_config = nullptr;
	// A pointer to the current visual info struct X11 uses to determine certain factors about the window
	XVisualInfo *visual_info = nullptr;

	// A pointer to the currently active window being used by the application
	GLWindow *_active_window = nullptr;

	bool using_vsync = false;

	Error _create_context(GLWindow &win);

public:
	XVisualInfo *get_visual_info();

	uint8_t create_window(Window *win);
	void destroy_window(uint8_t p_id);

	void set_active_window(uint8_t p_id);
	void release_active_window();

	bool is_using_vsync() const;
	void set_use_vsync(bool p_value);

	Error initialize();
	void finalize();
	void swap_buffers() const;

	void resize_viewport(uint16_t width, uint16_t height) const;

	/**
	 * @brief Class constructor. Currently does nothing.
	 */
	GLManagerX11() {}
	/**
	 * @brief Class destructor. Currently does nothing.
	 */
	~GLManagerX11() {}
};

#endif // PLATFORM_LINUX
