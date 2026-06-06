#include "editor_main.h"

#include <core/input/input.h>
#include <core/input/input_event.h>
#include <core/input/input_map.h>
#include <core/io/filesystem.h>
#include <core/io/resource_importer.h>
#include <core/os/os.h>
#include <core/string/print_string.h>

#include <rendering/render_data.h>
#include <rendering/rendering_manager.h>
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
			if (Input::get_singleton()->is_action_just_pressed("place_toggle")) {
				place_toggle = true;
			}

			if (place_toggle) {
				Vector2i pos = Input::get_singleton()->get_mouse_screen_position();
				GameObject *o = get_child(0);
				UIObject *ci = Object::cast_to<UIObject>(o);
				// We can 1) apply the position as an offset of the anchor, or 2) apply the position as an absolute.
				ci->set_position(pos);
				ci->set_rotation(OS::get_singleton()->get_os_running_time());
				place_toggle = false;
			}

			if (Input::get_singleton()->is_action_just_pressed("DEBUG_2")) {
				RenderData *rd = RM::get_singleton()->get_render_data();
				OS::get_singleton()->print("Draw calls: %d\nPrimitives: %d\nOverall time: %fms",
										   rd->draw_calls,
										   rd->primitive_count,
										   rd->render_time * 1000);
			}
		}
	}
}

Ref<Font> Editor::get_editor_font() {
	return editor_fonts[0];
}

Editor::Editor() {
	// Set CWD to that of the executable.
	String dir = OS::get_singleton()->get_executable_path().get_directory();
	FileSystem::get_singleton()->set_cwd(dir);

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

	InputMap::get_singleton()->add_action("place_toggle");
	Ref<InputEventMouseButton> btn;
	btn.instantiate();
	btn->button = InputEnums::MOUSE_LBUTTON;
	InputMap::get_singleton()->add_action_event("place_toggle", btn);

	Ref<InputEventKey> f3;
	f3.instantiate();
	f3->key = Key::F3;
	InputMap::get_singleton()->add_action_event("DEBUG_3", f3);

	Ref<InputEventKey> f2;
	f2.instantiate();
	f2->key = Key::F2;
	InputMap::get_singleton()->add_action_event("DEBUG_2", f2);
}

Editor::~Editor() {}
