#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#include "core/data/event.h"
#include "core/os/display_manager.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

class DisplayManagerWindows : public DisplayManager {
private:

    HINSTANCE hInstance = nullptr;

    struct WindowData {
        uint8 id;
        HWND hWnd;
        Event<WindowNotification, uint8> notification_callback;
        uint16 x, y, width, height;
    };

    WindowData *window = nullptr;
public:

    LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual uint8 create_window(const char *p_name, uint16 x, uint16 y, uint16 width, uint16 height) override;
    virtual void destroy_window(uint8 p_id) override;

    virtual void process_events() override;
    virtual void swap_buffers() override;
    
    virtual void finalize() override;

    DisplayManagerWindows();
    ~DisplayManagerWindows();
};

#endif // PLATFORM_WINDOWS