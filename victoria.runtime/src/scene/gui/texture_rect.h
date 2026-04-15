#pragma once

#include "scene/gui/ui_object.h"
#include "scene/resources/texture.h"

class VAPI TextureRect : public UIObject {
	VREGISTER_CLASS(TextureRect, UIObject);

	Ref<Texture> texture;

protected:
	void _notification(int p_what);

public:
	Ref<Texture> get_texture() const;
	void set_texture(const Ref<Texture> &p_tex);

	TextureRect() {}
};
