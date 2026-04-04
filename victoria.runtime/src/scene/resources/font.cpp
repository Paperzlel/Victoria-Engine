#include "scene/resources/font.h"

Font::Character Font::get_character(char c) const {
	RBMap<char, Character>::Element *e = font_map.find(c);
	ERR_COND_NULL_R(e, Font::Character());
	return e->value();
}

void Font::set_character(char c, const Character &p_char) {
	font_map.insert(c, p_char);
}

void Font::set_font_size(uint32_t p_size) {
	font_size = p_size;
}

uint32_t Font::get_max_font_height() const {
	return max_font_height;
}

void Font::set_max_font_height(uint32_t p_height) {
	max_font_height = p_height;
}

uint32_t Font::get_font_baseline_offset() const {
	return font_baseline_offset;
}
void Font::set_font_baseline_offset(uint32_t p_offset) {
	font_baseline_offset = p_offset;
}

uint32_t Font::get_bitmap_size() const {
	return bitmap_size;
}

void Font::set_bitmap_size(uint32_t p_size) {
	bitmap_size = p_size;
}

Font::Font() {}

Font::~Font() {}