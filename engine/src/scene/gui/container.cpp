#include "scene/gui/container.h"

ObjectUI *Container::as_gui_object(Object *p_obj) const {
    ObjectUI *u = Item::cast_to<ObjectUI>(p_obj);
    if (!u) {
        return nullptr;
    }

    return u;
}

void Container::fit_child_in_rect(ObjectUI *p_child, Vector2i p_pos, Vector2i p_size) {
    // Vector2i size = get_size();

    // Passes pre-determine size.

    p_child->set_size(p_size);
    p_child->set_position(p_pos);
}
