#include "scene/gui/vbox_container.h"

void VBoxContainer::_resize() {
    List<Object *> children = get_children();
    Vector2 parent_size = get_size();
    Vector2 parent_pos = get_transform().position;
    Vector2 cumulative_child_size = Vector2(parent_size.x, 0);
    for (Object *child : children) {
        ObjectUI *c = Item::cast_to<ObjectUI>(child);
        // Is not a GUI object, ignore.
        if (c == nullptr) {
            continue;
        }

        Vector2 size = c->get_size();
        // Resize children to fit in rect.
        if (size.x > parent_size.x) {
            size.x = parent_size.x;
            c->set_size(size);
        }

        if (cumulative_child_size.y + size.y >= parent_size.y) {
            parent_size.y = cumulative_child_size.y + size.y;
        }

        // Add separation and reposition child
        if (child != children.front()->get()) {
            cumulative_child_size.y += 4;
        }

        // Update parent transform prior to adding
        if (parent_size != get_size()) {
            set_size(parent_size);
        }

        c->set_position(Vector2i(parent_pos.x, cumulative_child_size.y));

        // TODO: Add separation size
        cumulative_child_size.y += size.y + 4;
    }

    if (parent_size != get_size()) {
        set_size(parent_size);
    }
}


void VBoxContainer::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_CHILD_ENTERED_TREE: {
            _resize();
        } break;
    }
}

VBoxContainer::VBoxContainer() {

}
