#pragma once

#include <core/typedefs.h>

#include <scene/gui/viewport_container.h>

class EditorViewport : public ViewportContainer {
    VREGISTER_CLASS(EditorViewport, ViewportContainer)

public:

    EditorViewport() {}
    ~EditorViewport() {}
};