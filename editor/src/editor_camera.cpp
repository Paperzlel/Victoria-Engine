#include "editor_camera.h"

#include <core/io/input.h>
#include <core/os/display_manager.h>
#include <core/os/os.h>

void EditorCamera::_update(double p_delta) {
    if (Input::get_singleton()->is_key_pressed(KEY_ESCAPE)) {
        DisplayManager::get_singleton()->toggle_mouse_mode(false);
    }
    if (Input::get_singleton()->is_mouse_button_pressed(MOUSE_LBUTTON)) {
        DisplayManager::get_singleton()->toggle_mouse_mode(true);
    }

    speed = 5.0 * p_delta;

    if (DisplayManager::get_singleton()->get_mouse_mode()) {

        if (Input::get_singleton()->is_key_pressed(KEY_W)) {
            o_position -= direction * speed;
        }
        if (Input::get_singleton()->is_key_pressed(KEY_S)) {
            o_position += direction * speed;
        }
        if (Input::get_singleton()->is_key_pressed(KEY_A)) {
            o_position += direction.cross(Vector3::up()).normalized() * speed;
        }
        if (Input::get_singleton()->is_key_pressed(KEY_D)) {
            o_position -= direction.cross(Vector3::up()).normalized() * speed;
        }
        if (Input::get_singleton()->is_key_pressed(KEY_SHIFT)) {
            o_position.y -= speed;
        }
        if (Input::get_singleton()->is_key_pressed(KEY_SPACEBAR)) {
            o_position.y += speed;
        }
    
        Vector2i mouse_motion;
        // First iteration
        if (first_frame) {
            mouse_motion = {0, 0};
            first_frame = false;
        } else {
            mouse_motion = Input::get_singleton()->get_mouse_motion();
        }
        
        offset += Vector2(mouse_motion) * 0.1;
        if (offset.y > 89.0f) {
            offset.y = 89.0f;
        }
        if (offset.y < -89.0f) {
            offset.y = -89.0f;
        }
    
        direction.x = Math::sin(-offset.x * Math::DEG_TO_RAD_MULTIPLIER) * Math::cos(offset.y * Math::DEG_TO_RAD_MULTIPLIER);
        direction.y = Math::sin(offset.y * Math::DEG_TO_RAD_MULTIPLIER);
        direction.z = Math::cos(-offset.x * Math::DEG_TO_RAD_MULTIPLIER) * Math::cos(offset.y * Math::DEG_TO_RAD_MULTIPLIER);
        direction.normalize();

        /**
         * Since position data cannot be held within a matrix that is looking at a given point, we instead use this system, whereby one holds
         * the actual position of the camera within the class itself, and the applied transform contains the (fake) position of the object. 
         */

        Transform3D view(Mat4::look_at(o_position, o_position + direction, Vector3::up()));

        set_global_transform(view);
    }
}

void EditorCamera::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_ENTER_TREE: {
            set_position(o_position);
        } break;
        case NOTIFICATION_UPDATE: {
            double p_delta = get_tree()->get_update_time();
            _update(p_delta);
        } break;
    }
    Camera3D::_notification(p_what);
}

EditorCamera::EditorCamera() {
    direction = Vector3(0, 0, -1);
    o_position = Vector3(0, 0, 5);
}

EditorCamera::~EditorCamera() {

}