#include "display_manager_windows.h"
#if PLATFORM_WINDOWS

#	include "os_windows.h"
#	include "wgl_detect_version.h"

#	include "core/config/callable_method_pointer.h"
#	include "core/io/input.h"
#	include "main/main.h"

#	include <drivers/opengl/rendering_server_gl.h>

void DisplayManagerWindows::finalize() {
	if (gl_manager_windows) {
		gl_manager_windows->finalize();
	}

	if (egl_manager_windows) {
		egl_manager_windows->finalize();
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
uint8_t DisplayManagerWindows::create_window(const String &p_name,
											 uint16_t x,
											 uint16_t y,
											 uint16_t width,
											 uint16_t height,
											 WindowFlags p_flags) {
	// TODO: Make this a little nicer - maybe format to snake case for the title?
	String class_name = "victoria_engine_window_class_" + p_name;

	WNDCLASSA wc = {0};
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

	uint32_t window_flags = WS_OVERLAPPEDWINDOW;
	if (!(p_flags & WINDOW_FLAG_INVISIBLE)) {
		window_flags |= WS_VISIBLE;
	}

	win_data->hWnd =
		CreateWindowExA(0, class_name, p_name, window_flags, x, y, width, height, NULL, NULL, hInstance, NULL);

	if (win_data->hWnd == NULL) {
		return INVALID_WINDOW_ID;
	}

	win_data->position = Vector2i(x, y);
	win_data->size = Vector2i(width, height);

	CallableMethod cm = static_callable_mp(_notification_callback);
	win_data->notification_callback.connect(cm, false);

	if (gl_manager_windows) {
		uint8_t id = gl_manager_windows->create_window(win_data->hWnd, hInstance);
		// Invalid window ID, need to return an error.
		if (id == INVALID_WINDOW_ID) {
			vdelete(gl_manager_windows);
			OS::get_singleton()
				->print_error(__FILE__, __FUNCTION__, __LINE__, "GLWindow was unable to be created,", "", ERROR_ERR);
			return ERR_CANT_CREATE;
		}
		gl_manager_windows->set_active_window(id);
		win_data->id = id;
	}

	if (egl_manager_windows) {
		uint8_t id = egl_manager_windows->create_window(GetDC(win_data->hWnd), win_data->hWnd);
		if (id == INVALID_WINDOW_ID) {
			vdelete(egl_manager_windows);
			ERR_FAIL_MSG_R("EGLWindow was unable to be created.", ERR_CANT_CREATE);
		}

		win_data->id = id;
	}

	window = win_data;
	return win_data->id;
}

void DisplayManagerWindows::destroy_window(uint8_t p_id) {
	if (window->id == p_id) {
		if (gl_manager_windows) {
			gl_manager_windows->destroy_window(p_id);
		}

		if (egl_manager_windows) {
			egl_manager_windows->destroy_window(p_id);
		}
		DestroyWindow(window->hWnd);
	}
}

void DisplayManagerWindows::set_use_vsync(bool p_value) {
	if (gl_manager_windows) {
		gl_manager_windows->set_use_vsync(p_value);
	}

	if (egl_manager_windows) {
		egl_manager_windows->set_use_vsync(p_value);
	}
}

Vector2i DisplayManagerWindows::get_window_rect() const {
	ERR_COND_NULL_R(window, Vector2i());
	return window->size;
}

void DisplayManagerWindows::set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) {
	window->window_resize_callback = p_method;
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
		POINT centre = {(LONG)window->size.x / 2, (LONG)window->size.y / 2};
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
	MSG msg = {0};
	// WARNING: This should be used instead of if (GetMessage) since we want to check the messages rather than to hang
	// if there isn't a message
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void DisplayManagerWindows::swap_buffers() {
	if (gl_manager_windows) {
		gl_manager_windows->swap_buffers();
	}

	if (egl_manager_windows) {
		egl_manager_windows->swap_buffers();
	}
}

LRESULT DisplayManagerWindows::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	uint8_t window_id = INVALID_WINDOW_ID;

	if (window) {
		window_id = window->id;
	}

	if (window_id == INVALID_WINDOW_ID) {
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
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
			uint32_t width = LOWORD(lParam);
			uint32_t height = HIWORD(lParam);
			if (gl_manager_windows) {
				gl_manager_windows->resize_viewport(width, height);
			}

			if (window->window_resize_callback.is_valid()) {
				window->window_resize_callback.call(width, height);
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
			// bool is_just_pressed = (lParam & (1U << 30)) == 0;

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
				Vector2i c(window->size / 2);

				old_x = c.x;
				old_y = c.y;

				// Don't need to update the cursor position if the cursor is at the centre of the screen
				if (pos == c) {
					centre = c;
				} else {
					centre = pos;
					POINT pos = {(int)c.x, (int)c.y};
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

DisplayManager *DisplayManagerWindows::create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	DisplayManager *ds = vnew(DisplayManagerWindows(p_renderer, p_size, r_error));
	return ds;
}

DisplayManagerWindows::DisplayManagerWindows(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	hInstance = static_cast<OSWindows *>(OS::get_singleton())->get_hinstance();
	if (!hInstance) {
		hInstance = GetModuleHandleA(NULL);
	}

	String driver = p_renderer;

	bool can_load_gl = false;
	if (driver == "opengl") {
		HashTable<String, Variant> gl_info = detect_wgl_version();
		can_load_gl = gl_info["version"].operator int() >= 30003;

		// Warn about native GL being unavailable.
		if (!can_load_gl) {
			ERR_WARN("Your video card does not support the required OpenGL version of 3.3, switching to ANGLE.");
			driver = "opengl_es";
		}
	}

	if (driver == "opengl_es") {
		egl_manager_windows = vnew(EGLManagerANGLE);

		Error err = egl_manager_windows->initialize();
		if (err != OK) {
			vdelete(egl_manager_windows);
			egl_manager_windows = nullptr;
			if (can_load_gl) {
				ERR_WARN("Your video card drivers do not appear to support ANGLE or the ANGLE dynamic libraries have "
						 "not been supplied. Switching to native OpenGL.");
				driver = "opengl";
			} else {
				*r_error = err;
				ERR_FAIL_MSG("Could not initialize the ANGLE drivers.");
			}
		}

		RenderingServerGL::make_default();
	}

	if (driver == "opengl") {
		gl_manager_windows = vnew(GLManagerWindows);

		Error err = gl_manager_windows->initialize();
		if (err != OK) {
			*r_error = err;
			ERR_FAIL_MSG(error_messages[err]);
		}

		RenderingServerGL::make_default();
	}

	create_window("Victoria Engine Window", 100, 100, p_size.x, p_size.y);
}

void DisplayManagerWindows::register_windows_driver() {
	_create_funcs[create_func_count].func = create_func;
	_create_funcs[create_func_count].name = String("windows");
	create_func_count++;
}

DisplayManagerWindows::~DisplayManagerWindows() {
	if (gl_manager_windows) {
		vdelete(gl_manager_windows);
		gl_manager_windows = nullptr;
	}

	if (egl_manager_windows) {
		vdelete(egl_manager_windows);
		egl_manager_windows = nullptr;
	}
}

#endif // PLATFORM_WINDOWS