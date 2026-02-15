#pragma once

#include <core/typedefs.h>
#include <scene/resources/font.h>

#include "object_ui.h"

class VAPI Text : public ObjectUI {
	VREGISTER_CLASS(Text, ObjectUI)

	struct Character {
		RID id;
		char lookup_char;
		Vector2 position;
	};

	String text;
	List<Character> text_charlist;
	Ref<Font> font;
protected:

	void _notification(int p_what);
public:

	void set_font(const Ref<Font> &p_font);

	String get_text() const;
	void set_text(const String &p_text);

	Text();
	~Text();
};
