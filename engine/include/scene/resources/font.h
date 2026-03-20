#pragma once

#include "core/typedefs.h"
#include "core/data/rb_map.h"
#include "core/io/resource.h"
#include "core/math/vector2i.h"


class VAPI Font : public Resource {
	VREGISTER_CLASS(Font, Resource)
public:

	struct Character {
		RID texture;
		Vector2i size;
		Vector2i bearing;
		Vector2i origin;
		u32 advance;
	};

	Character get_character(char c) const;
	void set_character(char c, const Character &p_char);
protected:

	RBMap<char, Character> font_map;
	u32 font_size = 48;
	u32 bitmap_size = 256;
	u32 max_font_height = 0;
	u32 font_baseline_offset = 0;
public:

	void set_font_size(u32 p_size);

	u32 get_max_font_height() const;
	void set_max_font_height(u32 p_height);

	u32 get_font_baseline_offset() const;
	void set_font_baseline_offset(u32 p_offset);

	u32 get_bitmap_size() const;
	void set_bitmap_size(u32 p_size);

	Font();
	~Font();
};
