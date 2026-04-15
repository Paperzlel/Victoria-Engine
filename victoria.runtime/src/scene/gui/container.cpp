#include "scene/gui/container.h"

UIObject *Container::as_gui_object(GameObject *p_obj) const {
	UIObject *u = Object::cast_to<UIObject>(p_obj);
	if (!u) {
		return nullptr;
	}

	return u;
}

void Container::fit_child_in_rect(UIObject *p_child, Vector2i p_pos, Vector2i p_size) {
	// Vector2i size = get_size();

	// Passes pre-determine size.

	p_child->set_size(p_size);
	p_child->set_position(p_pos);
}
