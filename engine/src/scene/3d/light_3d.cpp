#include "scene/3d/light_3d.h"

#include "servers/rendering_server.h"

RID Light3D::get_light() const {
	return light;
}

Vector3 Light3D::get_colour() const {
	return colour;
}

void Light3D::set_colour(const Vector3 &p_colour) {
	colour = p_colour;

	RS::get_singleton()->light_set_ambient(light, colour);
	RS::get_singleton()->light_set_diffuse(light, colour);
}

Vector3 Light3D::get_specular() const {
	return specular;
}

void Light3D::set_specular(const Vector3 &p_specular) {
	specular = p_specular;
	RS::get_singleton()->light_set_specular(light, specular);
}

Light3D::Light3D() {
	light = RS::get_singleton()->light_allocate();
	set_base(light);
	set_specular(Vector3(1, 1, 1));
	set_colour(Vector3(1, 1, 1));
}

Light3D::~Light3D() {
	RS::get_singleton()->light_free(light);
	set_base(RID());
}

/// Point light

float PointLight3D::get_range() const {
	return range;
}

void PointLight3D::set_range(float p_range) {
	range = p_range;
	RS::get_singleton()->light_set_range(light, range);
}

PointLight3D::PointLight3D() {
	RS::get_singleton()->light_set_type(light, RS::TYPE_POINT_LIGHT);
	set_range(10);
}

PointLight3D::~PointLight3D() {}

/// Directional light

DirectionalLight3D::DirectionalLight3D() {
	RS::get_singleton()->light_set_type(light, RS::TYPE_DIRECTIONAL_LIGHT);
}

DirectionalLight3D::~DirectionalLight3D() {}

/// Spot light

float SpotLight3D::get_inner_cone() const {
	return inner_cone;
}

void SpotLight3D::set_inner_cone(float p_radius) {
	inner_cone = p_radius;
	RS::get_singleton()->light_set_radii(light, Math::cos(inner_cone), Math::cos(outer_cone));
}

float SpotLight3D::get_outer_cone() const {
	return outer_cone;
}

void SpotLight3D::set_outer_cone(float p_radius) {
	outer_cone = p_radius;
	RS::get_singleton()->light_set_radii(light, Math::cos(inner_cone), Math::cos(outer_cone));
}

SpotLight3D::SpotLight3D() {
	RS::get_singleton()->light_set_type(light, RS::TYPE_SPOT_LIGHT);
	set_inner_cone(10 * Math::DEG_TO_RAD_MULTIPLIER);
	set_outer_cone(12.5 * Math::DEG_TO_RAD_MULTIPLIER);
	RS::get_singleton()->light_set_range(light, 10);
}

SpotLight3D::~SpotLight3D() {}