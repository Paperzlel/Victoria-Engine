#include "display_manager_x11.h"
#if PLATFORM_LINUX

#include "core/os/os.h"
#include "main/main.h"

/**
 * @brief Creates a window and assigns it as the active window. The width and height of a given window is calculated from the top-left, with the
 * x axis moving right on the screen and the y axis moving downwards.
 * @param p_name The title of the window, what appears across the top of the display
 * @param x The x position of the window
 * @param y The y position of the window
 * @param width The number of pixels wide the window will be
 * @param height The number of pixels tall the window will be
 * @returns The ID of the created window (NOTE: For now, this will remain to be 0 until multiple windows are supported)
 */
uint8 DisplayManagerX11::create_window(const char *p_name, uint16 x, uint16 y, uint16 width, uint16 height) {
    int event_masks = KeyPressMask | KeyReleaseMask | KeymapStateMask
    | PointerMotionMask | ButtonPressMask | ButtonReleaseMask
    | EnterWindowMask | LeaveWindowMask | ExposureMask;
    
    XSetWindowAttributes window_attribs;
    window_attribs.border_pixel = BlackPixel(display, screen_id);
    window_attribs.background_pixel = WhitePixel(display, screen_id);
    window_attribs.event_mask = event_masks;

    Window win = XCreateSimpleWindow(
        display,
        RootWindow(display, screen_id),
        x, y,
        width, height,
        1, 
        BlackPixel(display, screen_id),
        WhitePixel(display, screen_id)
    );

    WindowData *win_data = new WindowData;
    win_data->x = x;
    win_data->y = y;
    win_data->width = width;
    win_data->height = height;
    win_data->window_attribs = window_attribs;
    win_data->notification_callback.connect(_notification_callback, false);

    Atom wm_close_atom = XInternAtom(display, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(display, win, &wm_close_atom, 1);
    win_data->wm_close_atom = wm_close_atom;

    XStoreName(display, win, p_name);

    XClearWindow(display, win);
    XMapRaised(display, win);
    win_data->win = win;

    win_data->id = 0;
    window = win_data;
    return window->id;
}

void DisplayManagerX11::destroy_window(uint8 p_id) {
    if (!window) {
        return;
    }

    if (window->id == p_id) {
        XDestroyWindow(display, window->win);
        delete window;
        window = nullptr;
    }
}

void DisplayManagerX11::process_events() {
    if (!display) {
        return;
    }

    XEvent event;
    while (XPending(display)) {
        XNextEvent(display, &event);

        switch (event.type) {
            case ClientMessage: {
                if (event.xclient.data.l[0] == window->wm_close_atom) {
                    window->notification_callback.fire(NOTIFICATION_WM_WINDOW_CLOSE, window->id);
                    OS::get_singleton()->set_exit_code(0);
                    set_should_quit(true);
                    return;
                }
            } break;
            case DestroyNotify: {
                window->notification_callback.fire(NOTIFICATION_WM_WINDOW_CLOSE, window->id);
                OS::get_singleton()->set_exit_code(0);
                set_should_quit(true);
                return;
            } break;
        }
    }
}

void DisplayManagerX11::swap_buffers() {

}

void DisplayManagerX11::finalize() {
    if (window) {
        destroy_window(window->id);
    }

    if (display) {
        XCloseDisplay(display);
    }
}

DisplayManagerX11::DisplayManagerX11() {
    display = XOpenDisplay(nullptr);
    ERR_COND_NULL_MSG(display, "Could not open a connection to the X11 display server.");

    screen = DefaultScreenOfDisplay(display);
    screen_id = DefaultScreen(display);
}

DisplayManagerX11::~DisplayManagerX11() {
    
}

#endif // PLATFORM_LINUX