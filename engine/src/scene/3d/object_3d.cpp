#include "scene/3d/object_3d.h"

void Object3D::_propagate_transform_changed(Object3D *p_object) {
	if (!is_inside_tree()) {
		return;
	}

	List<Object *> children = get_children();

	for (Object *&obj : children) {
		Object3D *c = Item::cast_to<Object3D>(obj);
		if (c) {
			c->_propagate_transform_changed(p_object);
		}
	}

	data.transforms_dirty = true;
	notification(NOTIFICATION_TRANSFORM_CHANGED);
}

void Object3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			// Transforms don't update when the node isn't in the tree, so call to update them once they're added in
			_propagate_transform_changed(this);
		} break;
	}
}

Vector3 Object3D::get_position() const {
	return data.local_transform.position;
}

Quaternion Object3D::get_rotation() const {
	return data.local_transform.basis.get_quaternion();
}

Vector3 Object3D::get_scale() const {
	return data.local_transform.basis.get_scale();
}

void Object3D::set_position(const Vector3 &p_position) {
	data.local_transform.position = p_position;
	_propagate_transform_changed(this);
}

void Object3D::set_rotation(const Quaternion &p_rotation) {
	Vector3 p_scale = data.local_transform.basis.get_scale();
	data.local_transform.basis = p_rotation.to_rotation_matrix() * Mat4::from_scale(p_scale);
	_propagate_transform_changed(this);
}

void Object3D::set_scale(const Vector3 &p_scale) {
	Quaternion r = get_transform().basis.get_quaternion();
	data.local_transform.basis = r.to_rotation_matrix() * Mat4::from_scale(p_scale);
	_propagate_transform_changed(this);
}

Transform3D Object3D::get_transform() const {
	return data.local_transform;
}

Transform3D Object3D::get_global_transform() const {
	if (data.transforms_dirty) {
		Object *obj = get_parent();
		Object3D *parent = Item::cast_to<Object3D>(obj);
		Transform3D n_transform;
		if (parent) {
			n_transform = parent->get_global_transform() * data.local_transform;
		} else {
			n_transform = data.local_transform;
		}

		data.global_transform = n_transform;
	}
	return data.global_transform;
}

void Object3D::set_transform(const Transform3D &p_transform) {
	data.local_transform = p_transform;
	_propagate_transform_changed(this);
}

void Object3D::set_global_transform(const Transform3D &p_transform) {
	Transform3D t;
	Object *obj = get_parent();
	Object3D *parent = Item::cast_to<Object3D>(obj);
	if (parent) {
		t = parent->get_global_transform().inverse() * p_transform;
	} else {
		t = p_transform;
	}

	set_transform(t);
}

void Object3D::translate(const Vector3 &p_translation) {
	Transform3D t = get_transform();
	t.translate_local(p_translation);
	set_transform(t);
}

void Object3D::translate_global(const Vector3 &p_translation) {
	Transform3D t = get_global_transform();
	t.position += p_translation;
	set_global_transform(t);
}

void Object3D::rotate(const Quaternion &p_rotation) {
	Transform3D t = get_transform();
	t.rotate_local(p_rotation);
	set_transform(t);
}

void Object3D::rotate_global(const Quaternion &p_rotation) {
	Transform3D t = get_global_transform();
	t.basis.rotate(p_rotation);
	set_global_transform(t);
}

void Object3D::rotate_x(double p_angle) {
	Transform3D t = get_transform();
	t.basis.rotate(Vector3(1, 0, 0), p_angle);
	set_transform(t);
}

void Object3D::rotate_y(double p_angle) {
	Transform3D t = get_transform();
	t.basis.rotate(Vector3(0, 1, 0), p_angle);
	set_transform(t);
}

void Object3D::rotate_z(double p_angle) {
	Transform3D t = get_transform();
	t.basis.rotate(Vector3(0, 0, 1), p_angle);
	set_transform(t);
}

void Object3D::rotate_xyz(double p_x, double p_y, double p_z) {
	Transform3D t = get_transform();
	t.basis = Mat4::rotate_xyz(p_x, p_y, p_z);
	set_transform(t);
}

void Object3D::scale(const Vector3 &p_scale) {
	Transform3D t = get_transform();
	t.scale_local(p_scale);
	set_transform(t);
}

void Object3D::scale_global(const Vector3 &p_scale) {
	Transform3D t = get_global_transform();
	t.scale(p_scale);
	set_global_transform(t);
}

void Object3D::look_at(const Vector3 &p_target, const Vector3 &p_up) {
	Transform3D t = get_global_transform();
	Vector3 pos = t.position;
	look_at_from(pos, p_target, p_up);
}

void Object3D::look_at_from(const Vector3 &p_position, const Vector3 &p_target, const Vector3 &p_up) {
	Mat4 b = Mat4::look_at(p_position, p_target, p_up);
	set_global_transform(Transform3D(b, p_position));
}

Object3D::Object3D() {}

Object3D::~Object3D() {}