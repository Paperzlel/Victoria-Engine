#include "scene/main/viewport.h"

#include "rendering/rendering_manager.h"
#include "scene/3d/camera_3d.h"
#include "scene/main/canvas_item.h"
#include "scene/main/window.h"

#include <core/object/class_registry.h>

void Viewport::_size_changed() {
	texture_proxy->texture = RM::get_singleton()->viewport_get_texture(viewport);
	texture_proxy->format = Texture::FORMAT_RGBA;
	texture_proxy->width = data.viewport_size.x;
	texture_proxy->height = data.viewport_size.y;
	texture_proxy->channel_count = 4;
}

void Viewport::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			// Activate viewport

			if (get_parent()) {
				Viewport *parent = get_parent()->get_viewport();
				RM::get_singleton()->viewport_set_parent(viewport, parent->get_viewport_rid());
			}

			RM::get_singleton()->viewport_set_active(viewport, true);
		} break;
		case NOTIFICATION_EXIT_TREE: {
			// De-activate viewport, no longer in tree

			RM::get_singleton()->viewport_set_parent(viewport, RID());
			RM::get_singleton()->viewport_set_active(viewport, false);
		} break;
	}
}

bool Viewport::_set_camera_3d(Camera3D *p_camera) {
	bool ret = camera_3d == nullptr;
	// TODO: Camera removed notification
	camera_3d = p_camera;
	if (p_camera) {
		RM::get_singleton()->viewport_attach_camera(viewport, p_camera->get_camera());
	} else {
		RM::get_singleton()->viewport_attach_camera(viewport, RID());
	}

	return ret;
}

void Viewport::_propagate_size_changed(GameObject *p_object) {
	if (!is_inside_tree()) {
		return;
	}

	if (p_object != this) {
		CanvasItem *ci = Object::cast_to<CanvasItem>(p_object);

		if (ci) {
			ci->queue_redraw();
		}
	}

	int count = p_object->get_child_count();

	for (int i = 0; i < count; i++) {
		_propagate_size_changed(p_object->get_child(i));
	}
}

void Viewport::_bind_methods() {
	ClassRegistry::add_signal(get_class_name_static(), "size_changed");
}

Camera3D *Viewport::get_camera_3d() const {
	return camera_3d;
}

Ref<ViewportTexture> Viewport::get_texture() const {
	return texture_proxy;
}

Window *Viewport::get_base_window() const {
	if (!is_inside_tree()) {
		return nullptr; // Not in tree --> no root window yet set
	}

	return get_tree()->get_root();
}

Vector2i Viewport::get_viewport_size() const {
	return data.viewport_size;
}

void Viewport::set_viewport_size(const Vector2i &p_size) {
	if (data.min_size.x > p_size.x || data.min_size.y > p_size.y) {
		data.viewport_size = data.min_size;
	} else {
		data.viewport_size = p_size;
	}

	RM::get_singleton()->viewport_set_size(viewport, p_size);

	// Change viewport texture prior to notifying canvas items in case they need the texture
	_size_changed();

	emit_method(String("size_changed"));

	_propagate_size_changed(this);
}

Vector2i Viewport::get_viewport_offset() const {
	return data.viewport_offset;
}

void Viewport::set_viewport_offset(const Vector2i &p_offset) {
	data.viewport_offset = p_offset;
	RM::get_singleton()->viewport_set_position(viewport, p_offset);
}

Vector2i Viewport::get_minimum_size() const {
	return data.min_size;
}

void Viewport::set_minimum_size(const Vector2i &p_size) {
	if (data.viewport_size == Vector2i::zero()) {
		data.viewport_size = p_size;
	}

	data.min_size = p_size;
}

Viewport::Viewport() {
	viewport = RM::get_singleton()->viewport_allocate();
	canvas = RM::get_singleton()->canvas_allocate();
	RM::get_singleton()->viewport_attach_canvas(viewport, canvas);

	texture_proxy.instantiate();
	texture_proxy->viewport = this;
	texture_proxy->texture = RM::get_singleton()->viewport_get_texture(viewport);
}

Viewport::~Viewport() {
	RM::get_singleton()->canvas_free(canvas);
	RM::get_singleton()->viewport_free(viewport);
}