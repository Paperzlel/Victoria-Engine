#include "importers/resource_importer_font.h"
#include "rendering/rendering_server.h"

#include "scene/resources/font.h"

#ifdef FREETYPE_ENABLED
#	include <ft2build.h>
#	include FT_FREETYPE_H
#	include FT_OUTLINE_H
#endif

// TODO: REMOVE, ADD FLOOR/CEIL TO MATH LIB
#include <math.h>

#ifdef FREETYPE_ENABLED
static FT_Library freetype_lib;
#endif

Ref<Resource> ResourceFormatImporterFont::_import(const String &p_file, int p_argc, Variant *p_args) {
#ifdef FREETYPE_ENABLED
	if (freetype_lib == nullptr) {
		ERR_FAIL_COND_MSG_R(FT_Init_FreeType(&freetype_lib) != 0, "Unable to load FreeType library", Ref<Resource>());
	}

	RS *rs = static_cast<RS *>(RS::get_singleton());
	Ref<Font> f;
	f.instantiate();

	int font_height = 48;
	uint64_t bitmap_size = 256;
	if (p_argc >= 2) {
		font_height = p_args[1];
	}

	uint32_t po2 = next_po2(font_height);
	uint32_t base = find_log2(po2);
	uint32_t diff = (find_log2(bitmap_size) - base);
	if (diff <= 3) {
		bitmap_size <<= diff == 2 ? 1 : (diff == 1 ? 2 : 3); // Bitmap next power should always be 3 of the font
	}

	f->set_font_size(font_height);
	f->set_bitmap_size(bitmap_size);

	FT_Face face;
	ERR_FAIL_COND_MSG_R(FT_New_Face(freetype_lib, p_file.get_data(), 0, &face) != 0,
						vformat("Unable to load Freetype face from path %s", p_file.get_data()).get_data(),
						Ref<Resource>());

	FT_Set_Pixel_Sizes(face, 0, font_height);

	FT_GlyphSlot slot = face->glyph;

	if (p_argc >= 1 && p_args[0].operator bool() == true) {
		// Load font into a larger bitmap image

		RBMap<int, char> char_map;
		for (uint8_t c = 32; c < 127; c++) {
			char_map.insert(c, (char)c);
		}

		uint8_t *bitmap = (uint8_t *)Memory::vallocate_zeroed(bitmap_size * bitmap_size);
		uint32_t max_x = 0;
		uint32_t max_y = 0;
		uint32_t max_row = 0;
		int max_height_pos = 0;
		int max_height_neg = 0;
		int border = 1; // 1 px border

		RID r = rs->texture_allocate();
		if (p_argc >= 3 && p_args[2].operator bool() == true) {
			rs->texture_use_sdf(r, true);
		}

		for (uint8_t i = 0; i < char_map.size(); i++) {
			char character = char_map[i + 32];

			if (FT_Load_Char(face, character, FT_LOAD_RENDER) != 0) {
				ERR_WARN(vformat("Unable to load Freetype character %c", character).get_data());
			}

			if (p_argc >= 3 && p_args[2].operator bool() == true) {
				FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);
			}

			if (max_x * max_y >= bitmap_size * bitmap_size) {
				ERR_WARN("exceeded max size, returning now");
				return f;
			}

			FT_Bitmap bmp = slot->bitmap;

			if (bmp.buffer == nullptr) {
				Font::Character ch;
				ch.advance = slot->advance.x;

				f->set_character((char)(i + 32), ch);
				continue;
			}

			if (slot->bitmap_top > 0) {
				if (slot->bitmap_top > max_height_pos) {
					max_height_pos = slot->bitmap_top;
				}

				int neg_h = bmp.rows - slot->bitmap_top;
				if (neg_h > max_height_neg) {
					max_height_neg = neg_h;
				}
			} else {
				int neg_h = -slot->bitmap_top + bmp.rows;
				if (neg_h > max_height_neg) {
					max_height_neg = neg_h;
				}
			}

			if (max_x + bmp.width >= bitmap_size - 1) {
				max_y += max_row + border;
				max_x = 0;
				max_row = 0;
			}

			if (bmp.rows + max_y >= bitmap_size - 1) {
				ERR_FAIL_MSG_R("Bitmap size was too small for font size, returning an empty bitmap.", f);
			}

			if (bmp.rows > max_row) {
				max_row = bmp.rows;
			}

			for (uint8_t h = 0; h < bmp.rows; h++) {
				for (uint8_t w = 0; w < bmp.width; w++) {
					if ((h + max_y) * (w + max_x) > bitmap_size * bitmap_size) {
						ERR_WARN("Bitmap pixel does not exist");
					}

					bitmap[w + max_x + (h + max_y) * bitmap_size] = bmp.buffer[w + h * bmp.width];
				}
			}

			Font::Character ch;
			ch.texture = r;
			ch.size = Vector2i(bmp.width, bmp.rows);
			ch.bearing = Vector2i(slot->bitmap_left, slot->bitmap_top);
			ch.origin = Vector2i(max_x, max_y);
			ch.advance = slot->advance.x;
			f->set_character((char)(i + 32), ch);

			max_x += bmp.width + border;
		}

		f->set_max_font_height(max_height_pos + max_height_neg);
		f->set_font_baseline_offset(max_height_pos);

		rs->texture_set_from_data(r, bitmap, bitmap_size, bitmap_size, RS::FORMAT_R, RS::MASK_FILTER_NEAREST);

		Memory::vfree(bitmap);
	} else {
		for (uint8_t c = 0; c < 128; c++) {
			ERR_FAIL_COND_MSG_R(FT_Load_Char(face, c, FT_LOAD_RENDER) != 0,
								vformat("Unable to load Freetype character %c", (char)c).get_data(),
								Ref<Resource>());

			if (p_argc >= 3 && p_args[2].operator bool() == true) {
				FT_Render_Glyph(slot, FT_RENDER_MODE_SDF);
			}

			Font::Character ch;
			ch.texture = rs->texture_allocate();
			ch.size = Vector2i(slot->bitmap.width, slot->bitmap.rows);
			ch.bearing = Vector2i(slot->bitmap_left, slot->bitmap_top);
			ch.advance = slot->advance.x;

			if (!slot->bitmap.buffer) {
				f->set_character(c, ch);
				continue;
			}

			// Fix lines problem by using the MASK_FILTER_NEAREST. Saves some time at the cost of making the text
			// slightly off.
			rs->texture_use_sdf(ch.texture, true);
			rs->texture_set_from_data(ch.texture,
									  slot->bitmap.buffer,
									  ch.size.x,
									  ch.size.y,
									  RS::FORMAT_R,
									  RS::MASK_FILTER_NEAREST);

			f->set_character(c, ch);
		}
	}

	FT_Done_Face(face);
	return f;
#else
	return Ref<Resource>();
#endif
}

ResourceFormatImporterFont::ResourceFormatImporterFont() {
	supported_formats.push_back("ttf");
}

ResourceFormatImporterFont::~ResourceFormatImporterFont() {
#ifdef FREETYPE_ENABLED
	if (freetype_lib) {
		FT_Done_FreeType(freetype_lib);
		freetype_lib = nullptr;
	}
#endif
}