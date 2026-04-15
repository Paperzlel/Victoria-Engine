#include "scene/resources/material.h"

#include "rendering/rendering_manager.h"

RID Material::get_material() const {
	return material;
}

void Material::set_material(RID p_rid) {
	if (material.is_valid()) {
		RM::get_singleton()->material_free(material);
	}

	if (p_rid.is_null()) {
		material = RID();
	} else {
		material = p_rid;
	}
}

Vector4 Material::get_colour() const {
	return colour;
}

void Material::set_colour(const Vector4 &p_colour) {
	colour = p_colour;
	RM::get_singleton()->material_set_colour(material, colour);
}

Vector3 Material::get_specular() const {
	return specular;
}

void Material::set_specular(const Vector3 &p_specular) {
	specular = p_specular;
	RM::get_singleton()->material_set_specular(material, specular);
}

Ref<Texture> Material::get_texture() const {
	if (texture.is_valid()) {
		return texture;
	}

	return Ref<Texture>();
}

void Material::set_texture(const Ref<Texture> &p_tex) {
	if (texture == p_tex) {
		return;
	}

	if (texture.is_valid()) {
		texture.unref();
	}

	RID tex_rid = RID();
	if (p_tex.is_valid()) {
		texture = p_tex;
		tex_rid = p_tex->get_texture();
	}

	RM::get_singleton()->material_set_texture(material, tex_rid);
}

Material::Material() {
	material = RM::get_singleton()->material_allocate();
	set_colour(Vector4(1, 1, 1, 1));
	set_specular(Vector3(0.5, 0.5, 0.5));
}

Material::~Material() {
	if (material.is_valid()) {
		RM::get_singleton()->material_free(material);
	}

	material = RID();
}