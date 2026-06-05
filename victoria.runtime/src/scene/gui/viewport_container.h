#pragma once

#include "scene/gui/container.h"
#include "scene/main/viewport.h"

class VRUNTIME_API ViewportContainer : public Container {
	VREGISTER_CLASS(ViewportContainer, Container);

	Ref<ViewportTexture> viewport_texture;

protected:
	void _notification(int p_what);

public:
	ViewportContainer();
};