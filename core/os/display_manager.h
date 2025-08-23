#pragma once

#include "core/typedefs.h"
#include "core/error/error_macros.h"
#include "core/string/vstring.h"

class DisplayManager {
protected:

    static DisplayManager *singleton;
public:

    static DisplayManager *get_singleton();

    const u8 INVALID_WINDOW_ID = -1;

    enum WindowNotification {
        NOTIFICATION_WM_WINDOW_CLOSE,
        NOTIFICATION_WM_MOUSE_ENTER,
        NOTIFICATION_WM_MOUSE_EXIT,
    };

    static void _notification_callback(WindowNotification notification, u8 window_id);

    virtual u8 create_window(const String &p_name, u16 x, u16 y, u16 width, u16 height) = 0;
    virtual void destroy_window(u8 p_id) = 0;

    virtual void process_events() = 0;
    virtual void swap_buffers() = 0;
    
    virtual void finalize() = 0;

    DisplayManager();
};