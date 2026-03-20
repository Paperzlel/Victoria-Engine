#pragma once

#include "core/typedefs.h"
#include "scene/main/viewport.h"
#include "container.h"

class VAPI ViewportContainer : public Container {
    VREGISTER_CLASS(ViewportContainer, Container)

    Ref<ViewportTexture> viewport_texture;
protected:

    void _notification(int p_what);
public:

    ViewportContainer();
};