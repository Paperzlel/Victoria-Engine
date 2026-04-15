#pragma once

#include <core/data/rb_map.h>
#include <core/io/resource.h>
#include <core/math/vector2i.h>

class VAPI Font : public Resource {
	VREGISTER_CLASS(Font, Resource);

public:
	struct Character {
		RID texture;
		Vector2i size;
		Vector2i bearing;
		Vector2i origin;
		uint32_t advance;
	};

	Character get_character(char c) const;
	void set_character(char c, const Character &p_char);

protected:
	RBMap<char, Character> font_map;
	uint32_t font_size = 48;
	uint32_t bitmap_size = 256;
	uint32_t max_font_height = 0;
	uint32_t font_baseline_offset = 0;

public:
	void set_font_size(uint32_t p_size);

	uint32_t get_max_font_height() const;
	void set_max_font_height(uint32_t p_height);

	uint32_t get_font_baseline_offset() const;
	void set_font_baseline_offset(uint32_t p_offset);

	uint32_t get_bitmap_size() const;
	void set_bitmap_size(uint32_t p_size);

	Font();
	~Font();
};
