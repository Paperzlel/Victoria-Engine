#include "scene/gui/viewport_container.h"

void ViewportContainer::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_CHILD_ENTERED_TREE: {
            Viewport *c = Item::cast_to<Viewport>(get_child(0));
            if (c) {
                set_size(c->get_viewport_size());
                viewport_texture = c->get_texture();
                canvas_set_texture_rect(viewport_texture, get_transform().position, get_size());
            }
        } break;
        case NOTIFICATION_DRAW: {
            Viewport *c = Item::cast_to<Viewport>(get_child(0));
            
            if (c) {
                set_size(c->get_viewport_size());
                canvas_set_texture_rect(viewport_texture, get_transform().position, get_size());
            }
        } break;
    }
}

ViewportContainer::ViewportContainer() {
    canvas_flip_y(true);
}
