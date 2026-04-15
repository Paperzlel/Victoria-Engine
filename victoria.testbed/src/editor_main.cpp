#include "editor_main.h"

#include <core/io/input.h>
#include <core/io/resource_importer.h>

#include <scene/gui/colour_rect.h>
#include <scene/gui/text.h>
#include <scene/gui/vbox_container.h>
#include <scene/gui/viewport_container.h>
#include <scene/main/canvas_item.h>
#include <scene/main/window.h>

void Editor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_UPDATE: {
		}
	}
}

Ref<Font> Editor::get_editor_font() {
	return editor_fonts[0];
}

Editor::Editor() {
	Vector<Variant> args;
	args.push_back(true);
	args.push_back(32);

	Ref<Font> arial = ResourceImporter::get_singleton()->import("assets/Arial.ttf", args.size(), args.ptrw(), nullptr);
	editor_fonts.push_back(arial);

	// Create vbox + add viewport
	// VBoxContainer *parent = vnew(VBoxContainer);
	// parent->set_size(Vector2i(300, 30));
	// add_child(parent);

	// Text *text = vnew(Text);
	// text->set_font(arial);
	// text->set_text("Hello World!");
	// parent->add_child(text);

	// Text *text2 = vnew(Text);
	// text2->set_font(arial);
	// text2->set_text("This is fun!!");
	// parent->add_child(text2);

	ColourRect *crect = vnew(ColourRect);
	add_child(crect);
	crect->set_colour(Vector4(1.0, 0.5, 0.5, 1.0));
	crect->set_anchor_location(UIObject::ANCHOR_CENTERED);
}

Editor::~Editor() {}
