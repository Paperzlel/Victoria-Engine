#include "scene/gui/ui_object.h"

#include "rendering/rendering_manager.h"
#include "scene/main/viewport.h"

#include <core/object/class_registry.h>

void UIObject::_size_changed() {
	// Update size
	// Size change assumes that offsets haven't been grown properly.
	// Also assumes that the user isn't overriding size manually. Will need a better system in the future.
	Vector2i parent_size = get_parent_rect();
	int edges[2];
	for (int i = 0; i < 2; i++) {
		edges[i] = data.offsets[i] + (parent_size[i] * data.anchor_factor[i]);
	}

	Vector2i new_pos = Vector2i(edges[0], edges[1]);
	Vector2i new_size = data.size_cache;

	if (data.min_size.x > new_size.x) {
		new_size.x = data.min_size.x;
	}

	if (data.min_size.y > new_size.y) {
		new_size.y = data.min_size.y;
	}

	bool pos_changed = new_pos != data.pos_cache;
	bool size_changed = new_size != data.size_cache;

	if (pos_changed) {
		data.pos_cache = new_pos;
	}
	if (size_changed) {
		data.size_cache = new_size;
	}
}

void UIObject::_update_minimum_size() {
	UIObject *parent = Object::cast_to<UIObject>(get_parent());

	if (!parent) {
		return;
	}

	Vector2 parent_min_size = parent->data.min_size;
	if (parent_min_size.x > data.min_size.x || parent_min_size.y > data.min_size.y) {
		data.min_size = parent_min_size;
		_size_changed();
	}
}

void UIObject::_update_anchors(Anchor p_anchor, bool p_keep_position) {
	if (!is_inside_tree()) {
		return;
	}

	// X axis
	switch (p_anchor) {
		case ANCHOR_TOP_LEFT:
		case ANCHOR_MIDDLE_LEFT:
		case ANCHOR_BOTTOM_LEFT: {
			_update_anchor(AXIS_X, 0, p_keep_position);
		} break;
		case ANCHOR_TOP_MIDDLE:
		case ANCHOR_CENTERED:
		case ANCHOR_BOTTOM_MIDDLE: {
			_update_anchor(AXIS_X, 0.5, p_keep_position);
		} break;
		case ANCHOR_TOP_RIGHT:
		case ANCHOR_MIDDLE_RIGHT:
		case ANCHOR_BOTTOM_RIGHT: {
			_update_anchor(AXIS_X, 1, p_keep_position);
		} break;
	}

	// Y axis
	switch (p_anchor) {
		case ANCHOR_TOP_LEFT:
		case ANCHOR_TOP_MIDDLE:
		case ANCHOR_TOP_RIGHT: {
			_update_anchor(AXIS_Y, 0, p_keep_position);
		} break;
		case ANCHOR_MIDDLE_LEFT:
		case ANCHOR_CENTERED:
		case ANCHOR_MIDDLE_RIGHT: {
			_update_anchor(AXIS_Y, 0.5, p_keep_position);
		} break;
		case ANCHOR_BOTTOM_LEFT:
		case ANCHOR_BOTTOM_MIDDLE:
		case ANCHOR_BOTTOM_RIGHT: {
			_update_anchor(AXIS_Y, 1, p_keep_position);
		} break;
	}

	// Transform probably changed, force redraw
	if (is_inside_tree()) {
		force_redraw();
	}

	_size_changed();
}

void UIObject::_update_anchor(Axis p_axis, double p_factor, bool p_keep_position) {
	Vector2i parent_size = get_parent_rect();

	// New position = offset in parent size + offset if needed

	// If parent resizes, we can't call to re-draw anchors, they need to be cached until disturbed.
	// So, we can write the X/Y factor out to a vector, then re-calculate the below position.

	data.anchor_factor[p_axis & 1] = p_factor;
	int new_pos = parent_size[p_axis & 1] * p_factor + (p_keep_position ? data.pos_cache[p_axis & 1] : 0) -
				  data.size_cache[p_axis & 1] * p_factor;
	data.pos_cache[p_axis & 1] = new_pos;
}

void UIObject::_update_offsets(const Vector2i &p_position) {
	Vector2i parent_size = get_parent_rect();

	data.offsets[0] = p_position.x - (parent_size.x * data.anchor_factor[0]);
	data.offsets[1] = p_position.y - (parent_size.y * data.anchor_factor[1]);
}

void UIObject::_update_canvas_item_transform() {
	Transform2D t = get_transform();

	RM::get_singleton()->item_set_transform(get_canvas_item(), t);
}

void UIObject::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			GameObject *parent = get_parent();

			while (parent != nullptr) {
				UIObject *ui_p = Object::cast_to<UIObject>(parent);
				if (ui_p) {
					data.ui_parent = ui_p;
					break;
				}

				parent = parent->get_parent();
			}

			Viewport *v = get_viewport();
			if (v) {
				v->connect_method("size_changed", callable_mp(this, &UIObject::_size_changed));
			}

			_update_minimum_size();
			_size_changed();
			_update_anchors(anchor_location, true);
		} break;
		case NOTIFICATION_DRAW: {
			// Can't update anchors, because force_redraw is called to do that anyway.
			_update_canvas_item_transform();
		} break;
	}
}

Transform2D UIObject::get_transform() const {
	Transform2D ret = data.transform;
	ret.position = data.pos_cache;
	return ret;
}

void UIObject::set_transform(const Transform2D &p_transform) {
	data.transform = p_transform;
	_propagate_transform_changed(this);
}

Vector2i UIObject::get_position() const {
	return data.pos_cache;
}

void UIObject::set_position(const Vector2i &p_pos) {
	data.pos_cache = p_pos;

	_update_offsets(p_pos);
	_size_changed();
	_propagate_transform_changed(this);
}

double UIObject::get_rotation() const {
	return data.transform.get_rotation();
}

void UIObject::set_rotation(double p_angle) {
	data.transform.set_rotation(p_angle);
	_propagate_transform_changed(this);
}

Vector2 UIObject::get_scale() const {
	return data.transform.get_scale();
}

void UIObject::set_scale(const Vector2 &p_scale) {
	data.transform.set_scale(p_scale);
	_propagate_transform_changed(this);
}

UIObject::Anchor UIObject::get_anchor_location() const {
	return anchor_location;
}

void UIObject::set_anchor_location(Anchor p_location) {
	anchor_location = p_location;
	_update_anchors(anchor_location);
}

Vector2i UIObject::get_size() const {
	return data.size_cache;
}

void UIObject::set_size(const Vector2i &p_size) {
	data.size_cache = p_size;
	_size_changed();
}

Vector2i UIObject::get_parent_rect() const {
	if (!is_inside_tree()) {
		return Vector2i();
	}

	if (data.ui_parent) {
		return data.ui_parent->get_size();
	} else {
		return get_viewport()->get_viewport_size();
	}

	return Vector2i();
}

void UIObject::_bind_methods() {
	ClassRegistry::add_signal(get_class_name_static(), "size_changed");
}
