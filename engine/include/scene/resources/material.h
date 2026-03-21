#pragma once

#include "texture.h"

#include "core/io/resource.h"
#include "core/math/vector3.h"
#include "core/typedefs.h"

class VAPI Material : public Resource {
	VREGISTER_CLASS(Material, Resource)

	RID material;
	Vector4 colour;
	Vector3 specular;

	Ref<Texture> texture;

public:
	RID get_material() const;
	void set_material(RID p_rid);

	Vector4 get_colour() const;
	void set_colour(const Vector4 &p_colour);

	Vector3 get_specular() const;
	void set_specular(const Vector3 &p_specular);

	Ref<Texture> get_texture() const;
	void set_texture(const Ref<Texture> &p_tex);

	Material();
	~Material();
};