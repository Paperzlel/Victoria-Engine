#pragma once

#include "core/typedefs.h"
#include "core/error/error_macros.h"

class DisplayManager {
protected:

    static DisplayManager *singleton;
public:

    static DisplayManager *get_singleton();

    const uint8 INVALID_WINDOW_ID = -1;

    enum WindowNotification {
        NOTIFICATION_WM_WINDOW_CLOSE,
        NOTIFICATION_WM_MOUSE_ENTER,
        NOTIFICATION_WM_MOUSE_EXIT,
    };

    static void _notification_callback(WindowNotification notification, uint8 window_id);

    virtual uint8 create_window(const char *p_name, uint16 x, uint16 y, uint16 width, uint16 height) = 0;
    virtual void destroy_window(uint8 p_id) = 0;

    virtual void process_events() = 0;
    virtual void swap_buffers() = 0;
    
    virtual void finalize() = 0;

    DisplayManager();
};