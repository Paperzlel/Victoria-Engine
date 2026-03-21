#include "scene/gui/text.h"

#include "scene/main/viewport.h"
#include "servers/rendering_server.h"

void Text::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_TRANSFORM_CHANGED: {
			Transform2D t = get_transform();
			for (const Character &c : text_charlist) {
				Transform2D c_t = t;
				c_t.position += c.position;
				RS::get_singleton()->item_set_rect_offset(c.id, c_t.position);
			}
		} break;
		case NOTIFICATION_ENTER_TREE: {
			RID cv = get_viewport()->get_canvas_rid();
			for (const Character &c : text_charlist) {
				RS::get_singleton()->item_set_parent(c.id, cv);
			}
		} break;
	}
}

void Text::set_font(const Ref<Font> &p_font) {
	if (font.is_valid()) {
		font.unref();
	}

	if (p_font.is_null()) {
		ERR_WARN("Input font resource is null.");
		return;
	}

	font = p_font;
}

String Text::get_text() const {
	return text;
}

void Text::set_text(const String &p_text) {
	text = p_text;
	for (Character &c : text_charlist) {
		RS::get_singleton()->item_free(c.id);
	}
	text_charlist.clear();

	Transform2D t = get_global_transform();
	int x = t.position.x;
	int y = t.position.y;

	int max_y = font->get_max_font_height();
	int baseline = font->get_font_baseline_offset();
	Vector2 new_size;
	new_size.y = max_y;

	ERR_FAIL_COND_MSG(font.is_null(),
					  "Could not get a list of font rects as the current font resource does not exist.");

	for (int i = 0; i < p_text.length(); i++) {
		char c = p_text[i];
		Font::Character loaded_char = font->get_character(c);
		Character ch;

		ch.lookup_char = c;
		if (c == ' ') {
			x += (loaded_char.advance >> 6);
			new_size.x += (loaded_char.advance >> 6);
			continue;
		}
		ch.id = RS::get_singleton()->item_allocate();

		u32 bmp_size = font->get_bitmap_size();
		Vector2i pos = Vector2i(x + loaded_char.bearing.x, y + baseline - loaded_char.bearing.y);
		RS::get_singleton()->item_set_texture_rect(ch.id, loaded_char.texture, pos, loaded_char.size);
		RS::get_singleton()->item_set_uv_rect(ch.id,
											  (Vector2)loaded_char.origin / bmp_size,
											  (Vector2)loaded_char.size / bmp_size);
		RS::get_singleton()->item_set_ysort(ch.id, get_ysort());

		ch.position = pos;
		text_charlist.push_back(ch);
		x += (loaded_char.advance >> 6);

		new_size.x += (loaded_char.advance >> 6) + loaded_char.bearing.x;
	}

	set_size(new_size);
}

Text::Text() {
	RS::get_singleton()->item_set_parent(get_canvas_item(), RID());
	// set_skip_draw(true);
	set_ysort(1);
}

Text::~Text() {}
