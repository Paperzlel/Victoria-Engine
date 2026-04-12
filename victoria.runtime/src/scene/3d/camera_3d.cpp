#include "rendering/rendering_server.h"

#include "scene/3d/camera_3d.h"
#include "scene/main/viewport.h"

void Camera3D::_update_camera() {
	if (RS::get_singleton()) {
		// Only grab the data if it's needed (which is not most of the time)
		Vector2i rect = get_viewport()->get_viewport_size();
		float aspect_ratio = (float)rect.x / rect.y;
		RS::get_singleton()->camera_set_projection(camera,
												   Mat4::perspective(fovy, aspect_ratio, near_plane, far_plane));
	}
}

void Camera3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			bool first = SceneTree::get_singleton()->get_active_camera() == nullptr ? true : false;
			if (first) {
				SceneTree::get_singleton()->set_active_camera(this);
			}

			Viewport *v = get_viewport();
			v->_set_camera_3d(this);
		} break;
		case NOTIFICATION_EXIT_TREE: {
			if (this == SceneTree::get_singleton()->get_active_camera()) {
				SceneTree::get_singleton()->set_active_camera(nullptr);
			}
		}
		case NOTIFICATION_TRANSFORM_CHANGED: {
			RS::get_singleton()->camera_set_transform(get_camera(), get_camera_transform());
		} break;
	}
}

Transform3D Camera3D::get_camera_transform() const {
	Transform3D t = get_global_transform();
	t.position = t.position.inverse();
	return t;
}

void Camera3D::toggle_active(bool p_active) {
	is_active_camera = p_active;
}

bool Camera3D::is_active() {
	return is_active_camera;
}

RID Camera3D::get_camera() const {
	return camera;
}

float Camera3D::get_fovy() const {
	return fovy;
}

void Camera3D::set_fovy(float p_fov) {
	fovy = p_fov;
}

float Camera3D::get_near() const {
	return near_plane;
}

void Camera3D::set_near(float p_near) {
	near_plane = p_near;
}

float Camera3D::get_far() const {
	return far_plane;
}

void Camera3D::set_far(float p_far) {
	far_plane = p_far;
}

Camera3D::Camera3D() {
	camera = RS::get_singleton()->camera_allocate();
}

Camera3D::~Camera3D() {
	RS::get_singleton()->camera_free(camera);
}