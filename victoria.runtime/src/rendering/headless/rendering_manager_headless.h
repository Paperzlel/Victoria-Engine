#pragma once

#include "rendering/rendering_manager.h"

#include <core/typedefs.h>

/**
 * @brief A headless version of our renderer. All commands do nothing except return default values in order for the
 * renderer to satisfy its most basic of functions.
 */
class RenderingManagerHeadless : public RenderingManager {
private:
	static RenderingManager *_create() {
		return vnew(RenderingManagerHeadless);
	}

protected:
	virtual void _render_internal(RenderData *r_data) override;

public:
	static void make_default() {
		_create_func = _create;
	}

	/* 3D API */

	/* Instance API */

	virtual RID instance_allocate() override;
	virtual void instance_free(RID p_inst) override;
	virtual void instance_set_base(RID p_inst, RID p_base) override;
	virtual void instance_set_transform(RID p_inst, const Transform3D &p_transform) override;

	/* Mesh API */

	virtual RID mesh_allocate() override;
	virtual void mesh_free(RID p_rid) override;
	virtual void mesh_set_from_data(RID p_mesh, const MeshData &p_data) override;
	virtual void mesh_set_material(RID p_mesh, RID p_material) override;

	/* Material API */

	virtual RID material_allocate() override;
	virtual void material_free(RID p_material) override;
	virtual void material_set_colour(RID p_material, const Vector4 &p_colour) override;
	virtual void material_set_specular(RID p_material, const Vector3 &p_specular) override;
	virtual void material_set_texture(RID p_material, RID p_texture) override;

	/* Light API */

	virtual RID light_allocate() override;
	virtual void light_free(RID p_light) override;
	virtual void light_set_type(RID p_light, LightType p_type) override;
	virtual void light_set_ambient(RID p_light, const Vector3 &p_ambient) override;
	virtual void light_set_diffuse(RID p_light, const Vector3 &p_diffuse) override;
	virtual void light_set_specular(RID p_light, const Vector3 &p_specular) override;
	virtual void light_set_range(RID p_light, float p_range) override;
	virtual void light_set_radii(RID p_light, float p_inner_radius, float p_outer_radius) override;

	/* Camera API */

	virtual RID camera_allocate() override;
	virtual void camera_free(RID p_rid) override;
	virtual void camera_set_transform(RID p_rid, const Transform3D &p_transform) override;
	virtual void camera_set_projection(RID p_rid, const Mat4 &p_projection) override;

	/* 2D/Canvas API */

	/* CanvasItem API */

	virtual RID item_allocate() override;
	virtual void item_free(RID p_item) override;
	virtual void item_set_parent(RID p_item, RID p_parent) override;
	virtual void item_set_transform(RID p_item, const Transform2D &p_transform) override;
	virtual void item_set_colour(RID p_item, const Vector4 &p_colour) override;
	virtual void item_set_ysort(RID p_item, int p_sort) override;
	virtual void item_set_rect(RID p_item, const Vector2 &p_position, const Vector2 &p_size) override;
	virtual void item_set_rect_offset(RID p_item, const Vector2 &p_offset) override;
	virtual void item_set_flag(RID p_item, ItemFlag p_flag) override;
	virtual void
	item_set_texture_rect(RID p_item, RID p_texture, const Vector2 &p_position, const Vector2 &p_size) override;
	virtual void item_set_uv_rect(RID p_item, Vector2 p_offset, Vector2 p_size) override;
	virtual void item_set_mesh(RID p_item, RID p_mesh) override;
	virtual void item_set_material(RID p_item, RID p_material) override;

	/* Canvas API */

	virtual RID canvas_allocate() override;
	virtual void canvas_free(RID p_canvas) override;

	/* Texture API */

	virtual RID texture_allocate() override;
	virtual void texture_free(RID p_texture) override;
	virtual void texture_set_from_data(RID p_texture,
									   const uint8_t *p_data,
									   int p_width,
									   int p_height,
									   TextureFormat p_format,
									   TextureMask p_mask) override;
	virtual void texture_use_sdf(RID p_texture, bool p_value) override;

	/* Viewport API */

	virtual RID viewport_allocate() override;
	virtual void viewport_free(RID p_viewport) override;
	virtual void viewport_set_position(RID p_viewport, Vector2i p_position) override;
	virtual void viewport_set_size(RID p_viewport, Vector2i p_size) override;
	virtual void viewport_set_window(RID p_viewport, int p_window_id) override;
	virtual void viewport_attach_camera(RID p_viewport, RID p_camera) override;
	virtual void viewport_attach_canvas(RID p_viewport, RID p_canvas) override;
	virtual void viewport_set_parent(RID p_viewport, RID p_parent) override;
	virtual void viewport_set_active(RID p_viewport, bool p_active) override;
	virtual RID viewport_get_texture(RID p_viewport) override;

	virtual Error initialize() override;
	virtual void finalize() override;

	void free(RID p_rid);

	RenderingManagerHeadless();
	~RenderingManagerHeadless();
};
