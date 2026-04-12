#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#	include "core/data/vector.h"
#	include "core/string/vstring.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

class GLManagerWindows {
private:
	/**
	 * @brief Struct that contains all the core information our OpenGL functions need to know about a given window.
	 */
	struct GLWindow {
		uint8_t id;
		// The display ID is the ID that links a given window to a given display. When looking for an OpenGL context,
		// the display ID lets the code know which context to apply itself to.
		uint8_t display_id;

		HDC hDC;
		HWND hWnd;
		HGLRC gl_context;
	};

	int gl_version_major = 0, gl_version_minor = 0, gl_version_patch = 0;

	GLWindow *_active_window = nullptr;
	bool using_vsync = false;

	static int _configure_pixel_format(HDC hDC);
	Error _create_context(GLWindow &win);

	void _create_nvapi_profile();

public:
	uint8_t create_window(HWND hWnd, HINSTANCE hInstance);
	void destroy_window(uint8_t p_id);

	uint8_t get_active_window();
	void set_active_window(uint8_t p_id);
	void release_active_window();

	bool is_using_vsync() const;
	void set_use_vsync(bool p_value);

	Error initialize();
	void finalize();
	void swap_buffers() const;

	void resize_viewport(uint16_t width, uint16_t height) const;

	GLManagerWindows() {}
	~GLManagerWindows();
};

#endif // PLATFORM_WINDOWS