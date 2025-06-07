#include "display_manager_windows.h"
#if PLATFORM_WINDOWS

#include "os_windows.h"

#include "main/main.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    DisplayManagerWindows *ds = static_cast<DisplayManagerWindows *>(DisplayManager::get_singleton());
    if (ds) {
        return ds->WndProc(hWnd, uMsg, wParam, lParam);
    } else {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

/**
 * @brief Method to create an empty window at a given position and size.
 * @param p_name The name/title of the window
 * @param x The x position on the screen of the given window
 * @param y The y position on the screen of the given window
 * @param width The width of the window
 * @param height The height of the window
 * @return The ID of the current window
 */
uint8 DisplayManagerWindows::create_window(const String &p_name, uint16 x, uint16 y, uint16 width, uint16 height) {
    LPCSTR class_name = "victoria_engine_window_class";

    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = ::WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = class_name;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.style = CS_DBLCLKS | CS_OWNDC;

    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "Failed to create the window class.", "Error", MB_ICONEXCLAMATION | MB_OK);
        return INVALID_WINDOW_ID;
    }

    WindowData *new_win = new WindowData;
    new_win->hWnd = CreateWindowExA(
        0,
        class_name,
        p_name,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        x, y, width, height,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (new_win->hWnd == NULL) {
        return INVALID_WINDOW_ID;
    }

    new_win->id = 0;
    new_win->x = x;
    new_win->y = y;
    new_win->width = width;
    new_win->height = height;

    new_win->notification_callback.connect(_notification_callback, false);

    if (gl_manager_windows) {
        uint8 id = gl_manager_windows->create_window(new_win->hWnd, hInstance);

        if (id == (uint8)-1) {
            delete gl_manager_windows;
            OS::get_singleton()->print_error(__FILE__, __FUNCTION__, __LINE__, "GLWindow was unable to be created.");
            return ERR_CANT_CREATE;
        }

        gl_manager_windows->set_active_window(id);
        new_win->id = id;
    }

    window = new_win;
    return new_win->id;
}

void DisplayManagerWindows::destroy_window(uint8 p_id) {
    if (window->id == p_id) {
        if (gl_manager_windows) {
            gl_manager_windows->destroy_window(p_id);
        }
        DestroyWindow(window->hWnd);
    }
}

void DisplayManagerWindows::process_events() {
    // Check if a window exists, if not ignore all requests to prevent the application hanging
    if (!window) {
        return;
    }

    MSG msg = { 0 };
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void DisplayManagerWindows::swap_buffers() {
    if (gl_manager_windows) {
        gl_manager_windows->swap_buffers();
    }
}

void DisplayManagerWindows::finalize() {
    if (window) {
        destroy_window(window->id);
        delete window;
        window = nullptr;
    }
}

LRESULT DisplayManagerWindows::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    uint8 window_id = INVALID_WINDOW_ID;

    if (window) {
        window_id = window->id;
    }

    switch (uMsg) {
        case WM_CLOSE: {
            window->notification_callback.fire(NOTIFICATION_WM_WINDOW_CLOSE, window_id);
            OS::get_singleton()->set_exit_code(0);
            Main::set_should_quit(true);
            return 0;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: {
            uint32 width = LOWORD(lParam);
            uint32 height = HIWORD(lParam);
            if (gl_manager_windows) {
                gl_manager_windows->resize_viewport(width, height);
            }
        } break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DisplayManagerWindows::DisplayManagerWindows() {
    hInstance = static_cast<OSWindows *>(OS::get_singleton())->get_hinstance();
    if (!hInstance) {
        hInstance = GetModuleHandleA(NULL);
    }
    
    gl_manager_windows = new GLManagerWindows;

    create_window("Victoria Engine Window", 100, 100, 1280, 720);

    Error err = gl_manager_windows->initialize();
    ERR_FAIL_COND_MSG(err != OK, error_messages[err]);
}

DisplayManagerWindows::~DisplayManagerWindows() {
    delete gl_manager_windows;
}

#endif // PLATFORM_WINDOWS