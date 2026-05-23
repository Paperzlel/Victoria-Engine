#include "rendering_manager_headless.h"

void RenderingManagerHeadless::_render_internal(RenderData *r_data) {}

RID RenderingManagerHeadless::instance_allocate() {
	return RID();
}

void RenderingManagerHeadless::instance_free(RID p_inst) {}

void RenderingManagerHeadless::instance_set_base(RID p_inst, RID p_base) {}

void RenderingManagerHeadless::instance_set_transform(RID p_inst, const Transform3D &p_transform) {}

RID RenderingManagerHeadless::mesh_allocate() {
	return RID();
}

void RenderingManagerHeadless::mesh_free(RID p_rid) {}

void RenderingManagerHeadless::mesh_set_from_data(RID p_mesh, const MeshData &p_data) {}

void RenderingManagerHeadless::mesh_set_material(RID p_mesh, RID p_material) {}

RID RenderingManagerHeadless::material_allocate() {
	return RID();
}

void RenderingManagerHeadless::material_free(RID p_material) {}

void RenderingManagerHeadless::material_set_colour(RID p_material, const Vector4 &p_colour) {}

void RenderingManagerHeadless::material_set_specular(RID p_material, const Vector3 &p_specular) {}

void RenderingManagerHeadless::material_set_texture(RID p_material, RID p_texture) {}

RID RenderingManagerHeadless::light_allocate() {
	return RID();
}

void RenderingManagerHeadless::light_free(RID p_light) {}

void RenderingManagerHeadless::light_set_type(RID p_light, LightType p_type) {}

void RenderingManagerHeadless::light_set_ambient(RID p_light, const Vector3 &p_ambient) {}

void RenderingManagerHeadless::light_set_diffuse(RID p_light, const Vector3 &p_diffuse) {}

void RenderingManagerHeadless::light_set_specular(RID p_light, const Vector3 &p_specular) {}

void RenderingManagerHeadless::light_set_range(RID p_light, float p_range) {}

void RenderingManagerHeadless::light_set_radii(RID p_light, float p_inner_radius, float p_outer_radius) {}

RID RenderingManagerHeadless::camera_allocate() {
	return RID();
}

void RenderingManagerHeadless::camera_free(RID p_rid) {}

void RenderingManagerHeadless::camera_set_transform(RID p_rid, const Transform3D &p_transform) {}

void RenderingManagerHeadless::camera_set_projection(RID p_rid, const Mat4 &p_projection) {}

RID RenderingManagerHeadless::item_allocate() {
	return RID();
}

void RenderingManagerHeadless::item_free(RID p_item) {}

void RenderingManagerHeadless::item_set_parent(RID p_item, RID p_parent) {}

void RenderingManagerHeadless::item_set_transform(RID p_item, const Transform2D &p_transform) {}

void RenderingManagerHeadless::item_set_colour(RID p_item, const Vector4 &p_colour) {}

void RenderingManagerHeadless::item_set_ysort(RID p_item, int p_sort) {}

void RenderingManagerHeadless::item_set_rect(RID p_item, const Vector2 &p_position, const Vector2 &p_size) {}

void RenderingManagerHeadless::item_set_rect_offset(RID p_item, const Vector2 &p_offset) {}

void RenderingManagerHeadless::item_set_flag(RID p_item, ItemFlag p_flag) {}

void RenderingManagerHeadless::item_set_texture_rect(RID p_item,
													 RID p_texture,
													 const Vector2 &p_position,
													 const Vector2 &p_size) {}

void RenderingManagerHeadless::item_set_uv_rect(RID p_item, Vector2 p_offset, Vector2 p_size) {}

void RenderingManagerHeadless::item_set_mesh(RID p_item, RID p_mesh) {}

void RenderingManagerHeadless::item_set_material(RID p_item, RID p_material) {}

RID RenderingManagerHeadless::canvas_allocate() {
	return RID();
}
void RenderingManagerHeadless::canvas_free(RID p_canvas) {}

RID RenderingManagerHeadless::texture_allocate() {
	return RID();
}

void RenderingManagerHeadless::texture_free(RID p_texture) {}

void RenderingManagerHeadless::texture_set_from_data(RID p_texture,
													 const uint8_t *p_data,
													 int p_width,
													 int p_height,
													 TextureFormat p_format,
													 TextureMask p_mask) {}

void RenderingManagerHeadless::texture_use_sdf(RID p_texture, bool p_value) {}

RID RenderingManagerHeadless::viewport_allocate() {
	return RID();
}
void RenderingManagerHeadless::viewport_free(RID p_viewport) {}

void RenderingManagerHeadless::viewport_set_position(RID p_viewport, Vector2i p_position) {}

void RenderingManagerHeadless::viewport_set_size(RID p_viewport, Vector2i p_size) {}

void RenderingManagerHeadless::viewport_set_window(RID p_viewport, int p_window_id) {}

void RenderingManagerHeadless::viewport_attach_camera(RID p_viewport, RID p_camera) {}

void RenderingManagerHeadless::viewport_attach_canvas(RID p_viewport, RID p_canvas) {}

void RenderingManagerHeadless::viewport_set_parent(RID p_viewport, RID p_parent) {}

void RenderingManagerHeadless::viewport_set_active(RID p_viewport, bool p_active) {}

RID RenderingManagerHeadless::viewport_get_texture(RID p_viewport) {
	return RID();
}

Error RenderingManagerHeadless::initialize() {
	return OK;
}

void RenderingManagerHeadless::finalize() {}

void RenderingManagerHeadless::free(RID p_rid) {}

RenderingManagerHeadless::RenderingManagerHeadless() {}

RenderingManagerHeadless::~RenderingManagerHeadless() {}
