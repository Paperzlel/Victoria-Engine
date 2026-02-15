#include <scene/gui/panel.h>

void Panel::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			canvas_set_rect(get_transform().position, get_size());
			canvas_set_colour(colour);
		} break;
	}
}

Vector4 Panel::get_colour() const {
	return colour;
}

void Panel::set_colour(const Vector4 &p_colour) {
	colour = p_colour;

	force_redraw();
}
