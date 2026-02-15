#pragma once

#include <core/typedefs.h>
/**
 * The base class for our renderer. All actions that involve rendering something to a screen go through here, from materials to vertices. Since the
 * actual application of most of this code varies depending on if it's using OpenGL or Vulkan, we create a virtual class like this one to abstract
 * out all of the actual processing done.
 */

#include <core/data/rid_owner.h>
#include <core/error/error_macros.h>
#include <core/math/transform_2d.h>
#include <core/math/transform_3d.h>
#include <core/math/vector2.h>
#include <core/string/vstring.h>
#include <core/variant/variant_caster.h>

#include "rendering/render_data.h"

class VAPI RenderingServer {
protected:

    static RenderingServer *singleton;
    static RenderingServer *(*_create_func)();

    RenderData *frame_data = nullptr;

    virtual void _render_internal(RenderData *r_data) = 0;
public:

    static RenderingServer *get_singleton();

    static RenderingServer *create();

    /* Basic functionality */

    void draw();
    RenderData *get_render_data();


    virtual Error initialize() = 0;
    virtual void finalize() = 0;

    /* 3D/Scene API */

    enum {
        FORMAT_USE_2D_VERTICES = 1,
    };

    struct MeshData {
        Vector<Vector3> vertex_data;
        Vector<Vector3> normal_data;
        Vector<Vector2> uv_data;
        u32 vertex_count = 0;

        Vector<int> index_attribs;
        u32 index_count = 0;

        int normal_offset = 0;
        u64 format = 0;
    };

    /* Instance API */

    enum InstanceType {
        INSTANCE_TYPE_NONE,
        INSTANCE_TYPE_MESH,
        INSTANCE_TYPE_LIGHT,
    };

    virtual RID instance_allocate() = 0;
    virtual void instance_free(RID p_inst) = 0;
    virtual void instance_set_base(RID p_inst, RID p_base) = 0;
    virtual void instance_set_transform(RID p_inst, const Transform3D &p_transform) = 0;

    /* Mesh API */

    virtual RID mesh_allocate() = 0;
    virtual void mesh_free(RID p_mesh) = 0;
    virtual void mesh_set_from_data(RID p_mesh, const MeshData &p_data) = 0;
    virtual void mesh_set_material(RID p_mesh, RID p_material) = 0;

    /* Material API */

    virtual RID material_allocate() = 0;
    virtual void material_free(RID p_material) = 0;
    virtual void material_set_colour(RID p_material, const Vector4 &p_colour) = 0;
    virtual void material_set_specular(RID p_material, const Vector3 &p_specular) = 0;
    virtual void material_set_texture(RID p_material, RID p_texture) = 0;

    /* Camera API */

    virtual RID camera_allocate() = 0;
    virtual void camera_free(RID p_camera) = 0;
    virtual void camera_set_transform(RID p_camera, const Transform3D &p_transform) = 0;
    virtual void camera_set_projection(RID p_camera, const Mat4 &p_projection) = 0;

    /* Light API */

    enum LightType {
        TYPE_DIRECTIONAL_LIGHT,
        TYPE_POINT_LIGHT,
        TYPE_SPOT_LIGHT,
    };

    virtual RID light_allocate() = 0;
    virtual void light_free(RID p_light) = 0;
    virtual void light_set_type(RID p_light, LightType p_type) = 0;
    virtual void light_set_ambient(RID p_light, const Vector3 &p_ambient) = 0;
    virtual void light_set_diffuse(RID p_light, const Vector3 &p_diffuse) = 0;
    virtual void light_set_specular(RID p_light, const Vector3 &p_specular) = 0;
    virtual void light_set_range(RID p_light, float p_range) = 0;
    virtual void light_set_radii(RID p_light, float p_inner_radius, float p_outer_radius) = 0;


    /* 2D/Canvas API */

    /* CanvasItem API */

    enum ItemFlag {
        ITEM_FLAG_NONE = 0,
        ITEM_FLAG_ALPHA_ONLY = 1 << 0,
        ITEM_FLAG_USE_SDF = 1 << 1,
        ITEM_FLAG_USE_TEXTURE_SUBCOORDS = 1 << 2,
        ITEM_FLAG_FLIP_Y = 1 << 3,
    };

    virtual RID item_allocate() = 0;
    virtual void item_free(RID p_item) = 0;
    virtual void item_set_parent(RID p_item, RID p_parent) = 0;
    virtual void item_set_transform(RID p_item, const Transform2D &p_transform) = 0;
    virtual void item_set_colour(RID p_item, const Vector4 &p_colour) = 0;
    virtual void item_set_ysort(RID p_item, int p_sort) = 0;
    virtual void item_set_rect(RID p_item, const Vector2 &p_position, const Vector2 &p_size) = 0;
    virtual void item_set_rect_offset(RID p_item, const Vector2 &p_offset) = 0;
    virtual void item_set_flag(RID p_item, ItemFlag p_flag) = 0;
    virtual void item_set_texture_rect(RID p_item, RID p_texture, const Vector2 &p_position, const Vector2 &p_size) = 0;
    virtual void item_set_uv_rect(RID p_item, Vector2 p_offset, Vector2 p_size) = 0;
    virtual void item_set_mesh(RID p_item, RID p_mesh) = 0;
    virtual void item_set_material(RID p_item, RID p_material) = 0;

    /* Canvas API */

    virtual RID canvas_allocate() = 0;
    virtual void canvas_free(RID p_canvas) = 0;

    /* Texture API */

    enum TextureFormat {
        FORMAT_R,
        FORMAT_RGB,
        FORMAT_RGBA,
    };

    enum TextureMask {
        MASK_DEFAULT = 0,
        MASK_FILTER_NEAREST = 1 << 0,
        MASK_FILTER_NEAREST_MIPMAP_NEAREST = 1 << 1,
        MASK_FILTER_LINEAR_MIPMAP_NEAREST = 1 << 2,
        MASK_FILTER_NEAREST_MIPMAP_LINEAR = 1 << 3,
        MASK_FILTER_LINEAR_MIPMAP_LINEAR = 1 << 4,
        MASK_FILTER_LINEAR = 1 << 5,
        MASK_WRAP_CLAMP_TO_EDGE = 1 << 6,
        MASK_WRAP_CLAMP_TO_BORDER = 1 << 7,
        MASK_WRAP_MIRRORED_REPEAR = 1 << 8,
        MASK_WRAP_REPEAT = 1 << 9,
        MASK_WRAP_MIRROR_CLAMP_TO_EDGE = 1 << 10,
    };

    virtual RID texture_allocate() = 0;
    virtual void texture_free(RID p_texture) = 0;
    virtual void texture_set_from_data(RID p_texture, const u8 *p_data, int p_width, int p_height, TextureFormat p_format, TextureMask p_mask = MASK_DEFAULT) = 0;
    virtual void texture_use_sdf(RID p_texture, bool p_value) = 0;

    /* Viewport/Framebuffer API */

    virtual RID viewport_allocate() = 0;
    virtual void viewport_free(RID p_viewport) = 0;
    virtual void viewport_set_position(RID p_viewport, Vector2i p_position) = 0;
    virtual void viewport_set_size(RID p_viewport, Vector2i p_size) = 0;
    virtual void viewport_attach_camera(RID p_viewport, RID p_camera) = 0;
    virtual void viewport_attach_canvas(RID p_viewport, RID p_canvas) = 0;
    virtual void viewport_set_window(RID p_viewport, int p_window_id) = 0;
    virtual void viewport_set_parent(RID p_viewport, RID p_parent) = 0;
    virtual void viewport_set_active(RID p_viewport, bool p_active) = 0;
    virtual RID viewport_get_texture(RID p_viewport) = 0;

    /* Miscellaneous */

    RenderingServer();
    virtual ~RenderingServer();
};

typedef RenderingServer RS;

VARIANT_CAST_ENUM(RenderingServer::InstanceType);
VARIANT_CAST_ENUM(RenderingServer::LightType);
VARIANT_CAST_ENUM(RenderingServer::ItemFlag);
VARIANT_CAST_ENUM(RenderingServer::TextureFormat);
VARIANT_CAST_ENUM(RenderingServer::TextureMask);
