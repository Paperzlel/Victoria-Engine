#include "gl_manager_windows_angle.h"

#ifdef PLATFORM_WINDOWS

#	include <EGL/eglext_angle.h>
#	include <glad/egl.h>

EGLenum EGLManagerANGLE::_egl_get_platform_enum() {
	return EGL_PLATFORM_ANGLE_ANGLE;
}

EGLenum EGLManagerANGLE::_egl_get_platform_api() {
	return EGL_OPENGL_ES_API;
}

Vector<EGLAttrib> EGLManagerANGLE::_egl_get_platform_attribs() {
	Vector<EGLAttrib> ret;
	ret.push_back(EGL_PLATFORM_ANGLE_TYPE_ANGLE);
	ret.push_back(EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE);
	ret.push_back(EGL_NONE);
	return ret;
}

Vector<EGLint> EGLManagerANGLE::_egl_get_context_attribs() {
	Vector<EGLint> ret;
	ret.push_back(EGL_CONTEXT_CLIENT_VERSION);
	ret.push_back(3);
	ret.push_back(EGL_NONE);
	return ret;
}

void EGLManagerANGLE::resize_viewport(uint16_t width, uint16_t height) {
	eglWaitNative(EGL_CORE_NATIVE_ENGINE);
}

#endif