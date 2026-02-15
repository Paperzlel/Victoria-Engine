#pragma once

#include "core/typedefs.h"
#include "core/error/error_macros.h"
#include "core/string/vstring.h"

class DisplayManager {
protected:

    // Singleton instance for the DisplayManager class.
    static DisplayManager *singleton;
public:

    /**
     * @brief Obtains the singleton instance of the `DisplayManager` class. All functions
     * should be accessed via this method except those marked static.
     */
    static DisplayManager *get_singleton();

    // ID for an invalid or null window.
    const u8 INVALID_WINDOW_ID = -1;

    // The possible notifications that can be raised by the window manager.
    enum WindowNotification {
        NOTIFICATION_WM_WINDOW_CLOSE,
        NOTIFICATION_WM_MOUSE_ENTER,
        NOTIFICATION_WM_MOUSE_EXIT,
    };

    /**
     * @brief Static callback for any window notifications raised. Acts in a platform-agnostic
     * manner as all events act the same in their outcomes.
     * @param notification The notification that occured.
     * @param window_id The window that raised the notification.
     */
    static void _notification_callback(WindowNotification notification, u8 window_id);

    /**
     * @brief Creates a window.
     * @param p_name The title of the window to use.
     * @param x The starting X position of the window
     * @param y The starting Y position of the window
     * @param width The width of the window in pixels
     * @param height The height of the window in pixels
     * @return The internally stored ID of the window. Currently only 0 is returned, and -1 on error
     */
    virtual u8 create_window(const String &p_name, u16 x, u16 y, u16 width, u16 height) = 0;
    
    /**
     * @brief Destroys the window corresponding to the given window ID.
     * @param p_id The ID of the window to destroy.
     */
    virtual void destroy_window(u8 p_id) = 0;

    /**
     * @brief Processes any queued events that have occured (input, resizing, etc.)
     */
    virtual void process_events() = 0;

    /**
     * @brief Swaps the front and backbuffer around, essentially presenting the next frame to the user.
     */
    virtual void swap_buffers() = 0;
    
    /**
     * @brief Cleans up any references or dynamically allocated objects related to the manager and closes
     * any open windows or popups.
     */
    virtual void finalize() = 0;

    /**
     * @brief Class constructor. Sets singleton to the current instance.
     */
    DisplayManager();
};