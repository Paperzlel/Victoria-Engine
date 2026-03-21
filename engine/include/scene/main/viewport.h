#pragma once

#include "object.h"

#include "core/data/rid.h"
#include "core/math/vector2i.h"
#include "core/typedefs.h"
#include "scene/resources/texture.h"

class Window;
class Camera3D;

class VAPI ViewportTexture : public Texture {
	VREGISTER_CLASS(ViewportTexture, Texture)

	friend class Viewport;
	Viewport *viewport = nullptr;

public:
	ViewportTexture() {}
};

class VAPI Viewport : public Object {
	VREGISTER_CLASS(Viewport, Object)

	RID viewport;
	RID canvas;

	Ref<ViewportTexture> texture_proxy;
	Camera3D *camera_3d = nullptr;

	struct Data {
		Viewport *p_parent_viewport = nullptr;

		Vector2i viewport_offset;
		Vector2i viewport_size;

		Vector2i min_size;
	} data;

	void _size_changed();

protected:
	void _notification(int p_what);

	friend class Camera3D;
	bool _set_camera_3d(Camera3D *p_camera);

	void _propagate_size_changed(Object *p_object);

	static void _bind_methods();

public:
	Camera3D *get_camera_3d() const;

	FORCE_INLINE RID get_viewport_rid() const {
		return viewport;
	}
	FORCE_INLINE RID get_canvas_rid() const {
		return canvas;
	}

	Ref<ViewportTexture> get_texture() const;

	Window *get_base_window() const;

	Vector2i get_viewport_size() const;
	void set_viewport_size(const Vector2i &p_size);

	Vector2i get_viewport_offset() const;
	void set_viewport_offset(const Vector2i &p_offset);

	Vector2i get_minimum_size() const;
	void set_minimum_size(const Vector2i &p_size);

	Viewport();
	~Viewport();
};
