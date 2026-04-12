#pragma once

#include "scene/gui/object_ui.h"
#include "scene/resources/texture.h"

class VAPI TextureRect : public ObjectUI {
	VREGISTER_CLASS(TextureRect, CanvasItem)

	Ref<Texture> texture;

protected:
	void _notification(int p_what);

public:
	Ref<Texture> get_texture() const;
	void set_texture(const Ref<Texture> &p_tex);

	TextureRect() {}
};
