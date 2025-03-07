#include "display_manager.h"

DisplayManager *DisplayManager::singleton = nullptr;

DisplayManager *DisplayManager::get_singleton() {
    return singleton;
}

/**
 * @brief Our main notification callback centre for certain events that can be triggered in a window.
 * @param notification The type of event we need to process
 * @param window_id The current window's ID
 */
void DisplayManager::_notification_callback(WindowNotification notification, uint8 window_id) {
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