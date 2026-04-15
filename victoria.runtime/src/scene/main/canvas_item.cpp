#include "scene/main/canvas_item.h"

#include "rendering/rendering_server.h"
#include "scene/main/viewport.h"

void CanvasItem::_set_transforms_dirty() {
	transforms_dirty = true;
}

void CanvasItem::_propagate_transform_changed(CanvasItem *p_canvas_item) {
	if (!is_inside_tree()) {
		return;
	}

	List<GameObject *> children = get_children();

	for (GameObject *&obj : children) {
		CanvasItem *c = Object::cast_to<CanvasItem>(obj);

		if (c) {
			c->_propagate_transform_changed(p_canvas_item);
		}
	}

	_set_transforms_dirty();
	notification(NOTIFICATION_TRANSFORM_CHANGED);
}

void CanvasItem::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_propagate_transform_changed(this);

			// Add to canvas
			RID cv = get_viewport()->get_canvas_rid();
			if (cv.is_valid()) {
				RS::get_singleton()->item_set_parent(get_canvas_item(), cv);
			}

			force_redraw();
		} break;

		case NOTIFICATION_TRANSFORM_CHANGED: {
			RS::get_singleton()->item_set_transform(get_canvas_item(), get_global_transform());
		} break;
	}
}

RID CanvasItem::get_canvas_item() const {
	return item;
}

Transform2D CanvasItem::get_global_transform() const {
	CanvasItem *ci = Object::cast_to<CanvasItem>(get_parent());
	if (transforms_dirty) {
		Transform2D n_transform;
		if (ci) {
			n_transform = ci->get_global_transform() * get_transform();
		} else {
			n_transform = get_transform();
		}
		global_transform = n_transform;
	}

	return global_transform;
}

void CanvasItem::force_redraw() {
	if (!is_inside_tree()) {
		return;
	}

	if (!skip_draw) {
		notification(NOTIFICATION_DRAW);
	}
}

void CanvasItem::canvas_set_colour(const Vector4 &p_colour) {
	RS::get_singleton()->item_set_colour(get_canvas_item(), p_colour);
}

void CanvasItem::canvas_set_rect(const Vector2 &p_position, const Vector2 &p_size) {
	RS::get_singleton()->item_set_rect(get_canvas_item(), p_position, p_size);
}

void CanvasItem::canvas_set_texture_rect(const Ref<Texture> &p_tex, const Vector2 &p_position, const Vector2 &p_size) {
	if (!skip_draw) {
		RS::get_singleton()->item_set_texture_rect(get_canvas_item(), p_tex->get_texture(), p_position, p_size);
	}
}

void CanvasItem::canvas_set_mesh(const Ref<Mesh> &p_mesh) {
	if (!skip_draw) {
		RS::get_singleton()->item_set_mesh(item, p_mesh->get_mesh());
	}
}

void CanvasItem::canvas_flip_y(bool p_value) {
	RS::get_singleton()->item_set_flag(get_canvas_item(),
									   (p_value ? RenderingServer::ITEM_FLAG_FLIP_Y : RS::ITEM_FLAG_NONE));
}

void CanvasItem::canvas_set_material(const Ref<Material> &p_material) {
	if (!skip_draw) {
		RS::get_singleton()->item_set_material(item, p_material->get_material());
	}
}

int CanvasItem::get_ysort() const {
	return ysort;
}

void CanvasItem::set_ysort(int p_value) {
	ysort = p_value;
	RS::get_singleton()->item_set_ysort(get_canvas_item(), ysort);
}

bool CanvasItem::get_skip_draw() const {
	return skip_draw;
}

void CanvasItem::set_skip_draw(bool p_value) {
	skip_draw = p_value;
}

CanvasItem::CanvasItem() {
	item = RS::get_singleton()->item_allocate();
}

CanvasItem::~CanvasItem() {
	if (item.is_valid()) {
		RS::get_singleton()->item_free(item);
	}
}