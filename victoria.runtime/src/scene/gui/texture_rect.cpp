#include "scene/gui/texture_rect.h"

void TextureRect::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_DRAW: {
			if (texture.is_valid()) {
				canvas_set_texture_rect(texture, Vector2i(), get_size());
			}
		} break;
	}
}

Ref<Texture> TextureRect::get_texture() const {
	return texture;
}

void TextureRect::set_texture(const Ref<Texture> &p_tex) {
	texture = p_tex;

	force_redraw();
}
