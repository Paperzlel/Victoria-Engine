#pragma once

#include "core/data/vector.h"
#include "core/typedefs.h"

#include <glad/egl.h>

class EGLManager {
	struct EGL_Window {
		uint8_t display_id = -1;

		EGLSurface surface = EGL_NO_SURFACE;
	};

	struct EGL_Display {
		void *native_display = nullptr;

		EGLContext context = EGL_NO_CONTEXT;
		EGLDisplay egl_display = EGL_NO_DISPLAY;
		EGLConfig config = nullptr;
	};

	Vector<EGL_Display> displays;

	EGL_Window *window = nullptr;

	// bool use_vsync = true;
	int egl_version_major, egl_version_minor = 0;

	virtual EGLenum _egl_get_platform_enum() = 0;
	virtual EGLenum _egl_get_platform_api() = 0;
	virtual Vector<EGLAttrib> _egl_get_platform_attribs() = 0;
	virtual Vector<EGLint> _egl_get_context_attribs() = 0;

	Error _create_context(EGL_Display &p_display);

	int _get_display_id(void *p_native);

public:
	int get_native_visual_id(void *p_native);

	uint8_t create_window(void *p_native_display, void *p_native_window);
	void destroy_window(uint8_t p_id);

	Error initialize(void *p_platform_display = nullptr);
	Error open_display(void *p_native);
	void finalize();
	void swap_buffers() const;
	void resize_viewport(uint16_t p_width, uint16_t p_height);

	/**
	 * @brief Sets all displays to use vsync depending on the value given. Should change once multiple displays
	 * are actually supported.
	 * @param p_value Whether to use vsync or not.
	 */
	void set_use_vsync(bool p_value);

	EGLManager() {}
	virtual ~EGLManager() {}
};
