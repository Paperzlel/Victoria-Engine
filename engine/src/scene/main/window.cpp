#include <scene/main/window.h>

#include <core/config/callable_method_pointer.h>
#include <servers/rendering_server.h>

void Window::_window_resize_callback(u8 p_window_id) {
    ERR_FAIL_COND_MSG(window_id != p_window_id, "Scene window ID was not the same as DisplayManager window ID.");

    set_viewport_size(DisplayManager::get_singleton()->get_window_rect());
}

void Window::_notification(int p_what) {
    switch (p_what){
        case NOTIFICATION_ENTER_TREE: {
            if (!get_parent())  {
                // Is root window
                window_id = 0;
                set_viewport_size(DisplayManager::get_singleton()->get_window_rect());
                RS::get_singleton()->viewport_set_window(get_viewport_rid(), window_id);

                DisplayManager::get_singleton()->set_window_resize_callback(callable_mp(this, &Window::_window_resize_callback), window_id);
            }
        } break;
    }
}

Window::Window() {

}