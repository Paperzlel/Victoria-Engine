#pragma once

#include "core/typedefs.h"
#if PLATFORM_LINUX

#include "gl_manager_x11.h"

#include "core/os/display_manager.h"
#include "core/data/event.h"

#include <X11/Xlib.h>

class DisplayManagerX11 : public DisplayManager {
private:

    Display *display = nullptr;

    Screen *screen = nullptr;

    int screen_id = 0;

    GLManagerX11 *gl_manager_x11 = nullptr;

    struct WindowData {
        u8 id;
        Window win;
        XSetWindowAttributes window_attribs;
        Atom wm_close_atom;

        Event<WindowNotification, u8> notification_callback;

        u16 x, y, width, height;
    };

    WindowData *window = nullptr;
public:

    Display *get_display() const { return display; }

    Screen *get_screen() const { return screen; }

    int get_screen_id() const { return screen_id; }


    virtual u8 create_window(const String &p_name, u16 x, u16 y, u16 width, u16 height) override;
    virtual void destroy_window(u8 p_id) override;

    virtual void process_events() override;
    virtual void swap_buffers() override;
    
    virtual void finalize() override;

    DisplayManagerX11();
    ~DisplayManagerX11();
};

#endif // PLATFORM_LINUX