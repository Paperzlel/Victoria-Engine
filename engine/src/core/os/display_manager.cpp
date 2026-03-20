#include "core/os/display_manager.h"

DisplayManager *DisplayManager::singleton = nullptr;

DisplayManager::DisplayManagerCreationStruct DisplayManager::_create_funcs[MAX_CREATE_FUNCS];
int DisplayManager::create_func_count = 0;

DisplayManager *DisplayManager::get_singleton() {
    return singleton;
}

DisplayManager *DisplayManager::create(int p_func_idx, const String &p_renderer, const Vector2i &p_size, Error *r_error) {
    DisplayManager *ret = _create_funcs[p_func_idx].func(p_renderer, p_size, r_error);
    return ret;
}

int DisplayManager::get_creation_func_count() {
    return create_func_count;
}

const String &DisplayManager::get_creation_func_name(int p_id) {
    return _create_funcs[p_id].name;
}

/**
 * @brief Our main notification callback centre for certain events that can be triggered in a window.
 * @param notification The type of event we need to process
 * @param window_id The current window's ID
 */
void DisplayManager::_notification_callback(WindowNotification notification, u8 window_id) {
    switch (notification) {
        case NOTIFICATION_WM_WINDOW_CLOSE: {
            singleton->destroy_window(window_id);
        } break;
        default: {

        } break;
    }
}

DisplayManager::DisplayManager() {
    singleton = this;
}
