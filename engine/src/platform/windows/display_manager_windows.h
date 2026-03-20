#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#include "core/os/display_manager.h"
#include "core/data/event.h"

#include "gl_manager_windows.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

class DisplayManagerWindows : public DisplayManager {
private:

    HINSTANCE hInstance = nullptr;
    GLManagerWindows *gl_manager_windows = nullptr;

    struct WindowData {
        u8 id;
        HWND hWnd;
        Event<WindowNotification, u8> notification_callback;

        u16 x, y, width, height;
    };

    bool first_frame = true;
    u16 old_x = 0;
    u16 old_y = 0;
    Vector2i centre;

    WindowData *window = nullptr;
public:

    static DisplayManager *create_func(const String &p_renderer, int p_window_width, int p_window_height);
    static void register_windows_driver();

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual u8 create_window(const String &p_name, u16 x, u16 y, u16 width, u16 height, WindowFlags p_flags = WINDOW_FLAG_DEFAULT) override;
    virtual void destroy_window(u8 p_id) override;

    virtual void set_use_vsync(bool p_value) override;

    virtual Vector2i get_window_rect() const override;

    virtual void toggle_mouse_mode(bool p_mode) override;
    virtual bool get_mouse_mode() const override;
    
    virtual void process_events() override;
    virtual void swap_buffers() override;
    
    virtual void finalize() override;

    DisplayManagerWindows(const String &p_renderer, int p_window_width, int p_window_height);
    ~DisplayManagerWindows();
};

#endif // PLATFORM_WINDOWS