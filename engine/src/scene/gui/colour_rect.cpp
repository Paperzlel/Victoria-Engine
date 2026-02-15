#include <scene/gui/colour_rect.h>

void ColourRect::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_DRAW: {
            canvas_set_rect(get_transform().position, get_size());
            canvas_set_colour(colour);
        } break;
    }
}

Vector4 ColourRect::get_colour() const {
    return colour;
}

void ColourRect::set_colour(const Vector4 &p_colour) {
    colour = p_colour;
    
    force_redraw();
}
