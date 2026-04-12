#include "gl_manager_egl_wayland.h"

#if PLATFORM_LINUX
#	ifdef WAYLAND_ENABLED

EGLenum EGLManagerWayland::_egl_get_platform_enum() {
	return EGL_PLATFORM_WAYLAND_KHR;
}

EGLenum EGLManagerWayland::_egl_get_platform_api() {
	return EGL_OPENGL_API;
}

Vector<EGLAttrib> EGLManagerWayland::_egl_get_platform_attribs() {
	return Vector<EGLAttrib>();
}

Vector<EGLint> EGLManagerWayland::_egl_get_context_attribs() {
	Vector<EGLint> ret;
	ret.push_back(EGL_CONTEXT_CLIENT_VERSION);
	ret.push_back(3);
	ret.push_back(EGL_NONE);
	return ret;
}

#	endif // WAYLAND_ENABLED
#endif	   // PLATFORM_LINUX
