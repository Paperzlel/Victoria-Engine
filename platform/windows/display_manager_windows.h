#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#include "core/data/event.h"
#include "core/os/display_manager.h"

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

    WindowData *window = nullptr;
public:

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual u8 create_window(const String &p_name, u16 x, u16 y, u16 width, u16 height) override;
    virtual void destroy_window(u8 p_id) override;

    virtual void process_events() override;
    virtual void swap_buffers() override;
    
    virtual void finalize() override;

    DisplayManagerWindows();
    ~DisplayManagerWindows();
};

#endif // PLATFORM_WINDOWS