#pragma once

#include "utilities.h"

#include "rendering/opengl/shaders/canvas.gen.h"
#include "rendering/opengl/shaders/copy.gen.h"
#include "rendering/opengl/shaders/scene.gen.h"
#include "rendering/rendering_manager.h"

#include <core/data/rid_owner.h>

class RenderingManagerGL : public RenderingManager {
private:
	GL::Utilities *utils;

	bool use_depth_test = false;

	/* 3D Drawing API */

	struct Instance {
		RID base; // The derived version of whatever visual object we have in our scene
		RID self;
		InstanceType type;

		Transform3D transform;
	};

	RIDOwner<Instance> instance_owner;

	struct Mesh {
		RID material;

		uint32_t vertex_buffer;
		uint32_t element_buffer;
		uint32_t vertex_array;

		int vertex_count;
		int index_count;
		int format;
	};

	struct GeometryData {
		RID base;
		Transform3D transform;
		RID texture;

		Vector2 offset;
	};

	RIDOwner<Mesh> mesh_owner;

	struct MaterialData {
		float diffuse[4];
		float ambient[4];
		float specular[3];
		float shininess;
	};
	static_assert(sizeof(MaterialData) % 16 == 0, "Materials must be a multiple of 16 bytes in width");

	struct Material {
		MaterialData *data = nullptr;

		RID texture;
		uint32_t buffer = 0;
	};

	RIDOwner<Material> material_owner;

	struct Light {
		LightType type;

		Vector3 ambient;
		Vector3 diffuse;
		Vector3 specular;

		float range;

		float inner_radius;
		float outer_radius;
	};

	RIDOwner<Light> light_owner;

	struct Camera {
		Transform3D view;
		Mat4 projection;
	};

	RIDOwner<Camera> camera_owner;

	// Order the same as in scene_shader.vert.glsl
	struct SceneData {
		struct UBO {
			float projection[16];
			float view[16];
			float time;
			uint32_t directional_light_count;
			float pad[2];
		};
		static_assert(sizeof(UBO) % 16 == 0, "SceneDataGlobals must be a multiple of 16 bytes in width");

		struct PointLight {
			float position[4];
			float ambient[4];
			float diffuse[4];
			float specular[3];
			float range;
		};
		static_assert(sizeof(PointLight) % 16 == 0, "Point light data must be a multiple of 16 bytes in width");

		struct DirectionalLight {
			float direction[4];
			float ambient[4];
			float diffuse[4];
			float specular[4];
		};
		static_assert(sizeof(DirectionalLight) % 16 == 0,
					  "Directional light data must be a multiple of 16 bytes in width");

		struct SpotLight {
			float position[4];
			float direction[3];
			float inner_cut_off;

			float ambient[3];
			float outer_cut_off;
			float diffuse[4];

			float specular[3];
			float range;
		};
		static_assert(sizeof(SpotLight) % 16 == 0, "Spotlight data must be a multiple of 16 bytes in width");

		UBO ubo;
		uint32_t ubo_buffer = 0;

		PointLight *point_lights = nullptr;
		uint32_t point_light_count = 0;
		uint32_t point_light_buffer = 0;

		DirectionalLight *directional_lights = nullptr;
		uint32_t directional_light_buffer = 0;

		SpotLight *spot_lights = nullptr;
		uint32_t spot_light_count = 0;
		uint32_t spot_light_buffer = 0;
	} scene_data;

	/* 2D Drawing API */

	struct Item {
		RID self;
		RID parent;

		Transform2D transform;

		// Basic rect support
		mutable Vector2 size;
		Vector4 colour = Vector4(1, 1, 1, 1);
		int ysort = 0;
		int flags = 0;

		enum BaseType {
			TYPE_NONE,
			TYPE_RECT,
			TYPE_MESH,
		};

		enum ItemFlags {
			ITEM_FLAG_ALPHA_ONLY = 1 << 0,
			ITEM_FLAG_USE_SDF = 1 << 1,
			ITEM_FLAG_USE_TEXTURE_SUBCOORDS = 1 << 2,
			ITEM_FLAG_FLIP_Y = 1 << 3,
			ITEM_FLAG_USE_RECT = 1 << 4,
		};

		struct Base {
			BaseType type = TYPE_NONE;
			RID material;

			virtual ~Base() {}
		};

		struct Rect : public Base {
			RID texture;

			Vector2 rect_offset;
			Vector2 rect_size;

			Vector2 uv_offset;
			Vector2 uv_size;

			Rect() {
				type = TYPE_RECT;
			}
		};

		struct Mesh : public Base {
			RID mesh;

			Mesh() {
				type = TYPE_MESH;
			}
		};

		Base *base = nullptr;

		template <typename T>
		void alloc_base() {
			if (base) {
				return; // Already allocated
			}

			base = vnew(T);
		}

		~Item() {
			vdelete(base);
			base = nullptr;
		}
	};

	RIDOwner<Item> canvas_item_owner;

	struct Canvas {
		Vector<Item *> child_items;
	};

	RIDOwner<Canvas> canvas_owner;

	static constexpr uint32_t MAX_INSTANCE_DATA_COUNT = 1 << 14; // felt fancy
	static constexpr uint32_t MAX_BATCH_ITEM_COUNT = 1 << 8;

	struct CanvasInstanceData {
		float model[6];
		float rect[4];
		float masks[2];
		float subcoords[4];
		float colour[4];
	};

	static_assert(sizeof(CanvasInstanceData) % 80 == 0, "CanvasInstanceData must be 64 bytes in width");

	/**
	 * Holds the data for every instance of an object we wish to have.
	 * Batches will contain data that the renderer considers to be the same for every item, to prevent
	 * problems with changing data that is different for each item (i.e. materials and textures)
	 */
	struct CanvasBatch {
		uint32_t start = 0; // How far into the CanvasInstanceData buffer
		uint32_t instance_count = 0;
		Item::BaseType type = Item::TYPE_NONE;
		Item::Base *base = nullptr; // Only set for meshes
		int ysort = 0;

		RID texture;
		RID material;
	};

	struct CanvasData {
		struct UBO {
			float projection[16];
		};
		static_assert(sizeof(UBO) % 16 == 0, "The CanvasData UBO must be a multiple of 16 bytes in width");

		UBO ubo;
		uint32_t canvas_buffer = 0;

		Vector<CanvasBatch> batches;
		uint32_t current_batch = 0;

		CanvasInstanceData *canvas_instance_data = nullptr;
		GLuint canvas_instance_data_buffer = 0;
		uint32_t instance_data_offset = 0;

		uint32_t rect_vertex_array = 0;
		uint32_t rect_vertex_buffer = 0;
		uint32_t rect_index_buffer = 0;

		uint32_t screen_quad = 0;
		uint32_t screen_quad_array = 0;
	} canvas_data;

	struct Texture {
		uint32_t texture_buffer;
		TextureFormat format;
		bool use_sdf = false;
	};

	RIDOwner<Texture> texture_owner;

	struct Viewport {
		RID self;
		RID parent;

		RID camera;
		RID colour_texture;
		RID canvas;

		bool active = false;
		int viewport_to_window = -1;
		Vector2i position;
		Vector2i size;

		GLuint framebuffer_id = 0;
		GLuint texture_depth_id = 0;
	};

	RIDOwner<Viewport> viewport_owner;

	Vector<Viewport *> sorted_active_viewports;
	Vector<Viewport *> active_viewports;
	bool sorted_viewports_dirty = false;

	RID default_material = RID();
	RID default_texture = RID();

	struct Shaders {
		GLSceneShader scene_shader;
		GLCanvasShader canvas_shader;
		GLCopyShader copy_shader;
	} shaders;

	static bool use_gles_over_gl;

	static RenderingManager *_create() {
		return vnew(RenderingManagerGL);
	}

	void _enable_attributes(uint32_t p_start, uint32_t p_rate = 1);
	void _new_canvas_batch();

	Vector<Viewport *> _get_sorted_viewports();

	void _render_scene(RenderData *r_data, Viewport *p_viewport, Camera *p_camera);
	void _render_canvas(RenderData *r_data, Viewport *p_viewport, Canvas *p_canvas);

	void _prepare_viewport(Viewport *p_viewport);
	void _render_viewport(RenderData *r_data, Viewport *p_viewport);
	void _rebuild_viewport_texture(Viewport *p_viewport, Vector2i p_size);

protected:
	virtual void _render_internal(RenderData *r_data) override;

public:
	enum UniformBindingLocation {
		BINDING_SCENE_DATA,
		BINDING_CANVAS_DATA,
		BINDING_MATERIAL_DATA,
		BINDING_POINT_LIGHT_DATA,
		BINDING_DIRECTIONAL_LIGHT_DATA,
		BINDING_SPOT_LIGHT_DATA,
	};

	static bool is_gles_over_gl() {
		return use_gles_over_gl;
	}

	static void set_gles_over_gl(bool p_value) {
		use_gles_over_gl = p_value;
	}

	static void make_default(bool p_use_gles) {
		_create_func = _create;
		set_gles_over_gl(p_use_gles);
	}

	void set_depth_testing(bool p_value) {
		use_depth_test = p_value;
	}

	static void set_uniform_mat4(int p_location, const Mat4 &p_matrix);
	static void set_uniform_vec3(int p_location, const Vector3 &p_vector);
	static void transform2d_store_mat2x3(const Transform2D &p_transform, float *mat2x3);
	static void set_uniform_vec4(int p_location, const Vector4 &p_vector);

	/* 3D/Scene API */

	bool owns_mesh(RID p_mesh);
	bool owns_light(RID p_light);

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

	RenderingManagerGL();
	~RenderingManagerGL();
};
