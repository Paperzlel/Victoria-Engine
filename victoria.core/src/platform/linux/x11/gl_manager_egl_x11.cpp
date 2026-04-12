#include "gl_manager_egl_x11.h"

#if PLATFORM_LINUX

EGLenum EGLManagerX11::_egl_get_platform_enum() {
	return EGL_PLATFORM_X11_KHR;
}

EGLenum EGLManagerX11::_egl_get_platform_api() {
	return EGL_OPENGL_API;
}

Vector<EGLAttrib> EGLManagerX11::_egl_get_platform_attribs() {
	return Vector<EGLAttrib>();
}

Vector<EGLint> EGLManagerX11::_egl_get_context_attribs() {
	Vector<EGLint> context_attribs;
	context_attribs.push_back(EGL_CONTEXT_CLIENT_VERSION);
	context_attribs.push_back(3);
	context_attribs.push_back(EGL_NONE);

	return context_attribs;
}

#endif