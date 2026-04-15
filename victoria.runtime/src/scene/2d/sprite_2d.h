#pragma once

#include "scene/2d/game_object_2d.h"

class VAPI Sprite2D : public GameObject2D {
	VREGISTER_CLASS(Sprite2D, GameObject2D);

	Ref<Texture> texture;
	bool centred = false;

	void _get_dest_rect(Vector2 &p_position, Vector2 &p_size);

public:
	void request_redraw();

	Ref<Texture> get_texture() const;
	void set_texture(const Ref<Texture> &p_texture);

	bool is_centred() const;
	void set_centred(bool p_value);

	Sprite2D();
	~Sprite2D();
};