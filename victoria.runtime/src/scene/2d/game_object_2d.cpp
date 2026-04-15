#include "scene/2d/game_object_2d.h"

#include "rendering/rendering_manager.h"

void GameObject2D::_update_transform() {
	transform.basis.set_rotation(rotation);
	transform.basis.set_scale(t_scale);

	RM::get_singleton()->item_set_transform(get_canvas_item(), transform);
}

void GameObject2D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_propagate_transform_changed(this);
		} break;
		// Update the transforms if they are changed
		case NOTIFICATION_TRANSFORM_CHANGED: {
			RM::get_singleton()->item_set_transform(get_canvas_item(), get_global_transform());
		}
	}
}

Transform2D GameObject2D::get_transform() const {
	return transform;
}

void GameObject2D::set_transform(const Transform2D &p_transform) {
	transform = p_transform;

	RM::get_singleton()->item_set_transform(get_canvas_item(), transform);

	_propagate_transform_changed(this);
}

void GameObject2D::set_global_transform(const Transform2D &p_transform) {
	Transform2D t;

	CanvasItem *ci = Object::cast_to<CanvasItem>(get_parent());
	if (ci) {
		t = ci->get_global_transform().inverse() * p_transform;
	} else {
		t = p_transform;
	}

	set_transform(t);
}

Vector2 GameObject2D::get_position() const {
	return transform.position;
}

double GameObject2D::get_rotation() const {
	return rotation;
}

Vector2 GameObject2D::get_scale() const {
	return t_scale;
}

void GameObject2D::set_position(const Vector2 &p_position) {
	transform.position = p_position;
	_propagate_transform_changed(this);
}

void GameObject2D::set_rotation(double p_rotation) {
	rotation = p_rotation;
	_update_transform();
}

void GameObject2D::set_scale(const Vector2 &p_scale) {
	t_scale = p_scale;
	_update_transform();
}

Vector2 GameObject2D::get_global_position() const {
	return get_global_transform().position;
}

double GameObject2D::get_global_rotation() const {
	return get_global_transform().get_rotation();
}

Vector2 GameObject2D::get_global_scale() const {
	return get_global_transform().get_scale();
}

void GameObject2D::set_global_position(const Vector2 &p_position) {
	Transform2D t = get_global_transform();
	t.position = p_position;
	set_global_transform(t);
}

void GameObject2D::set_global_rotation(double p_rotation) {
	CanvasItem *parent = Object::cast_to<CanvasItem>(get_parent());
	if (parent) {
		Transform2D parent_transform = parent->get_global_transform();
		Transform2D new_transform = get_global_transform();
		new_transform.set_rotation(p_rotation);
		new_transform = parent_transform.inverse() * new_transform;
		set_rotation(new_transform.get_rotation());
	} else {
		set_rotation(p_rotation);
	}
}

void GameObject2D::set_global_scale(const Vector2 &p_scale) {
	CanvasItem *parent = Object::cast_to<CanvasItem>(get_parent());
	if (parent) {
		Transform2D parent_transform = parent->get_global_transform();
		Transform2D new_transform = get_global_transform();
		new_transform.set_scale(p_scale);
		new_transform = parent_transform.inverse() * new_transform;
		set_scale(new_transform.get_scale());
	} else {
		set_scale(p_scale);
	}
}

void GameObject2D::translate(const Vector2 &p_translation) {
	Transform2D t = get_transform();
	t.translate_local(p_translation);
	set_transform(t);
}

void GameObject2D::rotate(double p_rotation) {
	Transform2D t = get_transform();
	t.rotate_local(p_rotation);
	set_transform(t);
}

void GameObject2D::scale(const Vector2 &p_scale) {
	Transform2D t = get_transform();
	t.scale_local(p_scale);
	set_transform(t);
}

void GameObject2D::translate_global(const Vector2 &p_translation) {
	Transform2D t = get_global_transform();
	t.translate(p_translation);
	set_global_transform(t);
}

void GameObject2D::rotate_global(double p_rotation) {
	Transform2D t = get_global_transform();
	t.rotate(p_rotation);
	set_global_transform(t);
}

void GameObject2D::scale_global(const Vector2 &p_scale) {
	Transform2D t = get_global_transform();
	t.scale(p_scale);
	set_global_transform(t);
}

GameObject2D::GameObject2D() {}

GameObject2D::~GameObject2D() {}