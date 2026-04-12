#pragma once

#include <core/config/class_registry.h>
#include <core/typedefs.h>
#include <scene/gui/object_ui.h>
#include <scene/main/object.h>
#include <scene/resources/font.h>
#include <scene/resources/texture.h>

class Editor : public Object {
	VREGISTER_CLASS(Editor, Object);

	Vector<Ref<Font>> editor_fonts;

public:
	void _notification(int p_what);

	Ref<Font> get_editor_font();

	Editor();
	~Editor();
};