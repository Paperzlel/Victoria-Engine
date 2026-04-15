#pragma once

#include <core/object/class_registry.h>

#include <scene/gui/ui_object.h>
#include <scene/main/game_object.h>
#include <scene/resources/font.h>
#include <scene/resources/texture.h>

class Editor : public GameObject {
	VREGISTER_CLASS(Editor, GameObject);

	Vector<Ref<Font>> editor_fonts;

public:
	void _notification(int p_what);

	Ref<Font> get_editor_font();

	Editor();
	~Editor();
};