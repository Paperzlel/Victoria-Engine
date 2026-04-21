#include "editor_main.h"

#include <core/io/input.h>
#include <core/io/resource_importer.h>
#include <core/os/os.h>

#include <scene/gui/colour_rect.h>
#include <scene/gui/text.h>
#include <scene/gui/texture_rect.h>
#include <scene/gui/vbox_container.h>
#include <scene/gui/viewport_container.h>
#include <scene/main/canvas_item.h>
#include <scene/main/window.h>

void Editor::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_UPDATE: {
			static bool place_toggle = false;
			if (Input::get_singleton()->is_mouse_button_just_pressed(MOUSE_LBUTTON)) {
				place_toggle = true;
			}

			if (place_toggle) {
				Vector2i pos = Input::get_singleton()->get_mouse_screen_position();
				GameObject *o = get_child(0);
				UIObject *ci = Object::cast_to<UIObject>(o);
				// We can 1) apply the position as an offset of the anchor, or 2) apply the position as an absolute.
				ci->set_position(pos);
				ci->set_rotation(OS::get_singleton()->get_os_running_time());
				// print_verbose(pos);
				print_verbose(ci->get_position());
				place_toggle = false;
			}
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

	Ref<Texture> wood = ResourceImporter::get_singleton()->import("assets/container.jpg");

	ColourRect *crect = vnew(ColourRect);
	add_child(crect);
	crect->set_colour(Vector4(1.0, 0.5, 0.5, 1.0));
	crect->set_anchor_location(UIObject::ANCHOR_CENTERED);

	TextureRect *tr = vnew(TextureRect);
	add_child(tr);
	tr->set_texture(wood);
	tr->canvas_set_colour(Vector4(1.0, 0.5, 0.5, 1.0));
	tr->set_position(Vector2i(100, 300));
}

Editor::~Editor() {}
