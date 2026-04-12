#pragma once

#include "drivers/egl/egl_manager.h"

class EGLManagerANGLE : public EGLManager {
	virtual EGLenum _egl_get_platform_enum() override;
	virtual EGLenum _egl_get_platform_api() override;
	virtual Vector<EGLAttrib> _egl_get_platform_attribs() override;
	virtual Vector<EGLint> _egl_get_context_attribs() override;

public:
	virtual void resize_viewport(uint16_t width, uint16_t height) override;

	EGLManagerANGLE() {}
};