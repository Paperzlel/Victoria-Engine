#pragma once

#include "scene/gui/object_ui.h"

// Class needs to do the following:
// Clip children to the size it is
// Have an "order" mode (starts from left, right, middle)

class VAPI Container : public UIObject {
	VREGISTER_CLASS(Container, UIObject);

public:
	UIObject *as_gui_object(GameObject *p_obj) const;

	void fit_child_in_rect(UIObject *p_child, Vector2i p_pos, Vector2i p_size);

	Container() {}
};
