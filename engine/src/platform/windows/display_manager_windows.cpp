#include "display_manager_windows.h"
#if PLATFORM_WINDOWS

#include "os_windows.h"

#include <core/config/callable_method_pointer.h>
#include <core/io/input.h>
#include <main/main.h>
#include <modules/opengl/rendering_server_gl.h>

void DisplayManagerWindows::finalize() {
    if (gl_manager_windows) {
        gl_manager_windows->finalize();
    }

    if (window) {
        destroy_window(window->id);
        Memory::vfree(window); // Freeing here, because otherwise the function is called twice.
        window = nullptr;
    }
}

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
u8 DisplayManagerWindows::create_window(const String &p_name, u16 x, u16 y, u16 width, u16 height, WindowFlags p_flags) {
    // TODO: Make this a little nicer - maybe format to snake case for the title?
    String class_name = "victoria_engine_window_class_" + p_name; 

    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = ::WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = class_name.get_data();
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); // We manage the cursor ourselves
    wc.style = CS_DBLCLKS | CS_OWNDC;

    if (!RegisterClassA(&wc)) {
        MessageBoxA(NULL, "Failed to create the window class.", "Error", MB_ICONEXCLAMATION | MB_OK);
        return INVALID_WINDOW_ID;
    }

    WindowData *win_data = (WindowData *)Memory::vallocate(sizeof(WindowData));
    Memory::vzero(win_data, sizeof(WindowData));

    u32 window_flags = WS_OVERLAPPEDWINDOW;
    if (!(p_flags & WINDOW_FLAG_INVISIBLE)) {
        window_flags |= WS_VISIBLE;    
    }

    win_data->hWnd = CreateWindowExA(
        0, 
        class_name,
        p_name,
        window_flags,
        x, y, width, height,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (win_data->hWnd == NULL) {
        return INVALID_WINDOW_ID;
    }

    win_data->x = x;
    win_data->y = y;
    win_data->width = width;
    win_data->height = height;

    CallableMethod cm = static_callable_mp(_notification_callback);
    win_data->notification_callback.connect(cm, false);

    if (gl_manager_windows) {
        u8 id = gl_manager_windows->create_window(win_data->hWnd, hInstance);
        // Invalid window ID, need to return an error.
        if (id == (u8)-1) {
            vdelete(gl_manager_windows);
            OS::get_singleton()->print_error(__FILE__, __FUNCTION__, __LINE__, "GLWindow was unable to be created,", "", ERROR_ERR);
            return ERR_CANT_CREATE;
        }
        gl_manager_windows->set_active_window(id);
        win_data->id = id;
    }

    window = win_data;
    return win_data->id;
}

void DisplayManagerWindows::destroy_window(u8 p_id) {
    if (window->id == p_id) {
        if (gl_manager_windows) {
            gl_manager_windows->destroy_window(p_id);
        }
        DestroyWindow(window->hWnd);
    }
}

void DisplayManagerWindows::set_use_vsync(bool p_value) {
    if (gl_manager_windows) {
        gl_manager_windows->set_use_vsync(p_value);
    }
}

Vector2i DisplayManagerWindows::get_window_rect() const {
    ERR_COND_NULL_R(window, Vector2i());
    return Vector2i(window->width, window->height);
}

void DisplayManagerWindows::toggle_mouse_mode(bool p_mode) {
    // Set mouse as being captured
    if (p_mode) {
        SetCursor(nullptr);
        SetCapture(window->hWnd);
        RECT r;
        GetWindowRect(window->hWnd, &r);
        ClipCursor(&r);
        
        // Centre the cursor in the middle of the screen
        POINT centre = {window->width / 2, window->height / 2};
        ClientToScreen(window->hWnd, &centre);
        SetCursorPos(centre.x, centre.y);
    } else {
        ReleaseCapture();
        ClipCursor(nullptr);
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
    mouse_mode = p_mode;
}

bool DisplayManagerWindows::get_mouse_mode() const {
    return mouse_mode;
}

void DisplayManagerWindows::process_events() {
    // Check if a window exists, if not ignore all requests to prevent the application hanging
    if (!window) {
        return;
    }
    MSG msg = { 0 };
    // WARNING: This should be used instead of if (GetMessage) since we want to check the messages rather than to hang if there isn't a message
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


LRESULT DisplayManagerWindows::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    u8 window_id = INVALID_WINDOW_ID;

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
            u32 width = LOWORD(lParam);
            u32 height = HIWORD(lParam);
            if (gl_manager_windows) {
                gl_manager_windows->resize_viewport(width, height);
            }
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            // Since we override the syskey settings, make a custom binding for alt+f4
            if (Input::get_singleton()->is_key_pressed(KEY_ALT) && Input::get_singleton()->is_key_pressed(KEY_F4)) {
                SendMessage(hWnd, WM_CLOSE, wParam, lParam);
            }

            // Consider the following: 
            // - is the key being pressed in the given time period (i.e. before lParam << 30 == 1)
            // - is the key a system key
            // - are there modifiers
            // - etc..

            bool is_pressed = (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN);
            // Saved for later when I need it
            //bool is_just_pressed = (lParam & (1U << 30)) == 0;

            Ref<InputEventKey> key_event;
            key_event.instantiate();

            key_event->key = (Key)wParam;
            key_event->pressed = is_pressed;

            Input::get_singleton()->parse_input_event(key_event);
            return 0;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP: {
            bool is_pressed = (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN || uMsg == WM_MBUTTONDOWN);

            // TODO: double-clicks

            MouseButton button = MOUSE_MAX;
            switch (uMsg) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    button = MOUSE_LBUTTON;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    button = MOUSE_RBUTTON;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    button = MOUSE_MBUTTON;
                    break;
                default:
                    break;
            }
            
            Ref<InputEventMouseButton> mb;
            mb.instantiate();

            mb->pressed = is_pressed;
            mb->mb = button;

            Input::get_singleton()->parse_input_event(mb);
        } break;
        case WM_MOUSEMOVE: {
            int x_position = GET_X_LPARAM(lParam);
            int y_position = GET_Y_LPARAM(lParam);

            Vector2i pos(x_position, y_position);

            // Mouse is captured
            if (mouse_mode) {
                Vector2i c(window->width / 2, window->height / 2);

                old_x = c.x;
                old_y = c.y;

                // Don't need to update the cursor position if the cursor is at the centre of the screen
                if (pos == c) {
                    centre = c;
                } else {
                    centre = pos;
                    POINT pos = { (int)c.x, (int)c.y };
                    ClientToScreen(window->hWnd, &pos);
                    SetCursorPos(pos.x, pos.y);
                }

            }

            // Check if the first frame has been processed, which usually had the old x and y set to 0.
            if (first_frame) {
                old_x = pos.x;
                old_y = pos.y;
                first_frame = false;
            }

            Ref<InputEventMouseMotion> mm;
            mm.instantiate();

            mm->absolute = pos;
            mm->relative = pos - Vector2i(old_x, old_y);

            Input::get_singleton()->parse_input_event(mm);
            
            old_x = pos.x;
            old_y = pos.y;

        } break;
        case WM_MOUSEWHEEL: {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            if (delta != 0) {
                delta = delta > 0 ? 1 : -1;
            }

            Ref<InputEventMouseScroll> ms;
            ms.instantiate();

            ms->scroll = delta;

            Input::get_singleton()->parse_input_event(ms);
        } break;
        case WM_KILLFOCUS: {
            Input::get_singleton()->clear();
        } break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


DisplayManager *DisplayManagerWindows::create_func(const String &p_renderer, int p_window_width, int p_window_height) {
    DisplayManager *ds = vnew(DisplayManagerWindows(p_renderer, p_window_width, p_window_height));

    return ds;
}

DisplayManagerWindows::DisplayManagerWindows(const String &p_renderer, int p_window_width, int p_window_height) {
    hInstance = static_cast<OSWindows *>(OS::get_singleton())->get_hinstance();
    if (!hInstance) {
        hInstance = GetModuleHandleA(NULL);
    }

    if (p_renderer == "opengl") {
        gl_manager_windows = vnew(GLManagerWindows);

        Error err = gl_manager_windows->initialize();
        ERR_FAIL_COND_MSG(err != OK, error_messages[err]);

        RenderingServerGL::make_default();
    }

    create_window("Victoria Engine Window", 100, 100, p_window_width, p_window_height);
}

void DisplayManagerWindows::register_windows_driver() {
    _create_funcs[create_func_count] = create_func;
    create_func_count++;
}

DisplayManagerWindows::~DisplayManagerWindows() {
    vdelete(gl_manager_windows);
}

#endif // PLATFORM_WINDOWS