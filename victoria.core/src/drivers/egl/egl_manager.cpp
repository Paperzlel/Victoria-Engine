#include "drivers/egl/egl_manager.h"

#include "core/os/os.h"
#include "core/string/print_string.h"

#include <glad/gl.h>

Error EGLManager::_create_context(EGL_Display &p_display) {
	Vector<EGLint> ctx_attribs = _egl_get_context_attribs();
	/* clang-format off */
    EGLint attribs[] = {
        EGL_RED_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE
    };
	/* clang-format on */

	EGLint config_count = 0;

	eglChooseConfig(p_display.egl_display, attribs, &p_display.config, 1, &config_count);

	ERR_FAIL_COND_R(eglGetError() != EGL_SUCCESS, ERR_BUG);
	ERR_FAIL_COND_R(config_count == 0, ERR_CANT_CREATE);

	p_display.context = eglCreateContext(p_display.egl_display,
										 p_display.config,
										 EGL_NO_CONTEXT,
										 (ctx_attribs.size() > 0) ? ctx_attribs.ptr() : nullptr);
	ERR_FAIL_COND_MSG_R(p_display.context == EGL_NO_CONTEXT,
						vformat("Can't create an EGL context, error %d.", eglGetError()),
						ERR_CANT_CREATE);

	return OK;
}

int EGLManager::_get_display_id(void *p_native) {
	ERR_COND_NULL_R(p_native, -1);

	for (int i = 0; i < displays.size(); i++) {
		if (displays[i].native_display == p_native) {
			return i;
		}
	}

	EGL_Display display;
	display.native_display = p_native;
	EGLenum platform = _egl_get_platform_enum();
	Vector<EGLAttrib> attrib_list = _egl_get_platform_attribs();

	if (GLAD_EGL_VERSION_1_5) {
		display.egl_display = eglGetPlatformDisplay(platform,
													display.native_display,
													(attrib_list.size() > 0) ? attrib_list.ptr() : nullptr);
	} else if (GLAD_EGL_EXT_platform_base) {
#if EGL_EXT_platform_base
		Vector<EGLint> attribs;
		for (const EGLAttrib attr : attrib_list) {
			attribs.push_back((EGLint)attr);
		}

		display.egl_display = eglGetPlatformDisplayEXT(platform, display.native_display, attribs.ptr());
#endif
	} else {
		EGLNativeDisplayType *native = (EGLNativeDisplayType *)display.native_display;
		display.egl_display = eglGetDisplay(*native);
	}

	ERR_FAIL_COND_R(eglGetError() != EGL_SUCCESS, -1);
	ERR_FAIL_COND_MSG_R(display.egl_display == EGL_NO_DISPLAY, "Can't create an EGL display.", -1);

	if (!eglInitialize(display.egl_display, nullptr, nullptr)) {
		ERR_FAIL_MSG_R("Can't initialize an EGL display.", -1);
	}

	EGLenum platform_api = _egl_get_platform_api();
	if (!eglBindAPI(platform_api)) {
		ERR_FAIL_MSG_R("OpenGL not supported.", -1);
	}

	Error err = _create_context(display);

	if (err != OK) {
		eglTerminate(display.egl_display);
		ERR_FAIL_R(-1);
	}

	displays.push_back(display);

	return displays.size() - 1;
}

int EGLManager::get_native_visual_id(void *p_native) {
	int display_id = _get_display_id(p_native);
	ERR_FAIL_COND_R(display_id < 0, -1);

	EGL_Display display = displays[display_id];

	EGLint native_id = -1;

	if (!eglGetConfigAttrib(display.egl_display, display.config, EGL_NATIVE_VISUAL_ID, &native_id)) {
		ERR_FAIL_MSG_R(vformat("Could not get the config attrib, error message %d.", eglGetError()), -1);
	}

	return native_id;
}

uint8_t EGLManager::create_window(void *p_native_display, void *p_native_window) {
	int disp_id = _get_display_id(p_native_display);

	if (disp_id < 0) {
		return -1;
	}

	EGL_Window *win = vnew(EGL_Window);
	win->display_id = disp_id;
	EGL_Display &display = displays[disp_id];

	if (GLAD_EGL_VERSION_1_5) {
		win->surface = eglCreatePlatformWindowSurface(display.egl_display, display.config, p_native_window, nullptr);

	} else if (GLAD_EGL_EXT_platform_base) {
#ifdef EGL_EXT_platform_base
		win->surface =
			eglCreatePlatformWindowSurfaceEXT(display.egl_display, display.config, p_native_window, nullptr);
#endif
	} else {
		EGLNativeWindowType *native_window_type = (EGLNativeWindowType *)p_native_window;
		win->surface = eglCreateWindowSurface(display.egl_display, display.config, *native_window_type, nullptr);
	}

	if (win->surface == EGL_NO_SURFACE) {
		ERR_FAIL_MSG_R("Can't create the window surface.", -1);
	}

	eglMakeCurrent(display.egl_display, win->surface, win->surface, display.context);

	window = win;
	return 0;
}

void EGLManager::destroy_window(uint8_t p_id) {
	if (!window) {
		return;
	}

	if (p_id == 0) {
		EGL_Display &d = displays[p_id];

		eglMakeCurrent(d.egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroySurface(d.egl_display, window->surface);
		vdelete(window);
		window = nullptr;
	}
}

Error EGLManager::initialize(void *p_platform_display) {
	if (!gladLoaderLoadEGL(EGL_NO_DISPLAY)) {
		OS::get_singleton()->printerr("EGL: Failed to load EGL.");
		return ERR_CANT_CREATE;
	}

	NativeDisplayType *native_display = (NativeDisplayType *)p_platform_display;

	EGLenum platform = _egl_get_platform_enum();
	Vector<EGLAttrib> attrib_list = _egl_get_platform_attribs();
	EGLDisplay disp = EGL_NO_DISPLAY;

	if (GLAD_EGL_EXT_platform_base) {
#ifdef EGL_EXT_platform_base
		Vector<EGLint> attribs;
		for (const EGLAttrib attr : attrib_list) {
			attribs.push_back((EGLint)attr);
		}

		disp = eglGetPlatformDisplayEXT(platform, p_platform_display, attribs.ptr());
#endif
	} else {
		disp = eglGetDisplay(*native_display);
	}

	if (disp == EGL_NO_DISPLAY) {
		eglTerminate(disp);
		OS::get_singleton()->print("Hmm, default display shenanigans :P");
		return ERR_CANT_CREATE;
	}

	eglInitialize(disp, nullptr, nullptr);

	int ver = gladLoaderLoadEGL(disp);

	if (!ver) {
		eglTerminate(disp);
		return ERR_CANT_CREATE;
	}

	int egl_ver_major = GLAD_VERSION_MAJOR(ver);
	int egl_ver_minor = GLAD_VERSION_MINOR(ver);

	print_verbose(vformat("EGL version %d.%d", egl_ver_major, egl_ver_minor));

	egl_version_major = egl_ver_major;
	egl_version_minor = egl_ver_minor;

	eglTerminate(disp);

	return OK;
}

Error EGLManager::open_display(void *p_native) {
	int id = _get_display_id(p_native);
	if (id < 0) {
		return ERR_CANT_CREATE;
	} else {
		return OK;
	}
}

void EGLManager::finalize() {
	for (int i = 0; i < displays.size(); i++) {
		eglDestroyContext(displays[i].egl_display, displays[i].context);
		eglTerminate(displays[i].egl_display);
	}
}

void EGLManager::swap_buffers() const {
	if (window) {
		EGL_Display disp = displays[window->display_id];

		eglSwapBuffers(disp.egl_display, window->surface);
	}
}

void EGLManager::set_use_vsync(bool p_value) {
	for (int i = 0; i < displays.size(); i++) {
		eglSwapInterval(displays[i].egl_display, p_value ? 1 : 0);
	}
}

void EGLManager::resize_viewport(uint16_t p_width, uint16_t p_height) {}
