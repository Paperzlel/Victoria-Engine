#include "display_manager_x11.h"
#if PLATFORM_LINUX

#	include "core/os/os.h"
#	include "drivers/opengl/rendering_server_gl.h"
#	include "main/main.h"

#	include <stdlib.h>

// Should be in a library but w/e
#	define XC_left_ptr 68

void DisplayManagerX11::_update_window(XEvent *p_event) {
	if (!p_event) {
		return;
	}

	Vector2i new_position(p_event->xconfigurerequest.x, p_event->xconfigurerequest.y);
	Vector2i new_size(p_event->xconfigurerequest.width, p_event->xconfigurerequest.height);

	if (new_position == window->position && new_size == window->size) {
		return;
	}

	window->position = new_position;
	window->size = new_size;

	if (window->window_resize_callback.is_valid()) {
		window->window_resize_callback.call(window->id);
	}
}

void DisplayManagerX11::_update_wm_properties() {
	Atom wm_atom;
	int format;
	unsigned long bytes_after;
	unsigned long state_count;
	Atom *states = nullptr;

	Vector<Atom> local_states;

	if (XGetWindowProperty(display,
						   window->win,
						   window->wm_state_atom,
						   0,
						   1024,
						   false,
						   AnyPropertyType,
						   &wm_atom,
						   &format,
						   &state_count,
						   &bytes_after,
						   (uint8_t **)&states) == Success) {
		local_states.resize(state_count);
		for (uint64_t i = 0; i < state_count; i++) {
			local_states[i] = states[i];
		}
	}

	Atom minimized_atom = XInternAtom(display, "_NET_WM_STATE_HIDDEN", false);
	Atom maximised_x = XInternAtom(display, "_NET_WM_STATE_HORZ", false);
	Atom maximised_y = XInternAtom(display, "_NET_WM_STATE_MAXIMISED_VERT", false);

	window->maximised = local_states.has(maximised_x) && local_states.has(maximised_y);
	window->minimized = local_states.has(minimized_atom);

	OS::get_singleton()->set_is_suspended(window->minimized);
}

/**
 * @brief Creates a window and assigns it as the active window. The width and height of a given window is calculated
 * from the top-left, with the x axis moving right on the screen and the y axis moving downwards.
 * @param p_name The title of the window, what appears across the top of the display
 * @param x The x position of the window
 * @param y The y position of the window
 * @param width The number of pixels wide the window will be
 * @param height The number of pixels tall the window will be
 * @returns The ID of the created window (NOTE: For now, this will remain to be 0 until multiple windows are supported)
 */
uint8_t DisplayManagerX11::create_window(const String &p_name,
										 uint16_t x,
										 uint16_t y,
										 uint16_t width,
										 uint16_t height,
										 WindowFlags p_flags) {
	XVisualInfo visual;
	if (gl_manager_x11) {
		visual = *gl_manager_x11->get_visual_info();
	}

	if (egl_manager_x11) {
		int visual_id = egl_manager_x11->get_native_visual_id(display);
		XVisualInfo vinfo_tmp;
		vinfo_tmp.visualid = (VisualID)visual_id;

		int info_count = 0;
		XVisualInfo *infos = XGetVisualInfo(display, VisualIDMask, &vinfo_tmp, &info_count);

		visual = infos[0];
		XFree(infos);
	}

	int event_masks = KeyPressMask | KeyReleaseMask | KeymapStateMask | PointerMotionMask | ButtonPressMask |
					  ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | ExposureMask | StructureNotifyMask |
					  PropertyChangeMask;

	int value_masks = CWBackPixel | CWColormap | CWBorderPixel | CWEventMask;

	XSetWindowAttributes window_attribs;
	window_attribs.border_pixel = BlackPixel(display, screen_id);
	window_attribs.background_pixel = WhitePixel(display, screen_id);
	window_attribs.override_redirect = true;
	window_attribs.colormap = XCreateColormap(display, RootWindow(display, screen_id), visual.visual, AllocNone);
	window_attribs.event_mask = event_masks;

	Window win = XCreateWindow(display,
							   RootWindow(display, screen_id),
							   x,
							   y,
							   width,
							   height,
							   0,
							   visual.depth,
							   InputOutput,
							   visual.visual,
							   value_masks,
							   &window_attribs);

	WindowData *win_data = vnew(WindowData);
	win_data->position = Vector2i(x, y);
	win_data->size = Vector2i(width, height);
	win_data->window_attribs = window_attribs;

	CallableMethod cm = static_callable_mp(_notification_callback);
	win_data->notification_callback.connect(cm, false);

	Atom wm_close_atom = XInternAtom(display, "WM_DELETE_WINDOW", true);
	XSetWMProtocols(display, win, &wm_close_atom, 1);
	win_data->wm_close_atom = wm_close_atom;

	win_data->wm_state_atom = XInternAtom(display, "_NET_WM_STATE", true);

	XStoreName(display, win, p_name);

	XClearWindow(display, win);

	if (!(p_flags & WINDOW_FLAG_INVISIBLE)) {
		XMapRaised(display, win);
	}

	win_data->win = win;

	if (gl_manager_x11) {
		uint8_t id = gl_manager_x11->create_window(&win_data->win);
		if (id == (uint8_t)-1) {
			vdelete(gl_manager_x11);
			OS::get_singleton()->print_error(__FILE__, FUNCTION_STR, __LINE__, "GLWindow was unable to be created.");
			return INVALID_WINDOW_ID;
		}
		win_data->id = id;
	}

	if (egl_manager_x11) {
		uint8_t id = egl_manager_x11->create_window(display, &win_data->win);
		if (id == (uint8_t)-1) {
			vdelete(egl_manager_x11);
			OS::get_singleton()->print_error(__FILE__, FUNCTION_STR, __LINE__, "EGLWindow was unable to be created,");
			return INVALID_WINDOW_ID;
		}

		win_data->id = id;
	}

	window = win_data;
	return window->id;
}

/**
 * @brief Deletes the currently active window.
 * @param p_id The current ID of the active window
 */
void DisplayManagerX11::destroy_window(uint8_t p_id) {
	if (!window) {
		return;
	}

	if (window->id == p_id) {
		if (gl_manager_x11) {
			gl_manager_x11->destroy_window(p_id);
		}

		if (egl_manager_x11) {
			egl_manager_x11->destroy_window(p_id);
		}

		XFreeColormap(display, window->window_attribs.colormap);
		XDestroyWindow(display, window->win);
		vdelete(window);
		window = nullptr;
	}
}

void DisplayManagerX11::set_use_vsync(bool p_value) {
	if (gl_manager_x11) {
		gl_manager_x11->set_use_vsync(p_value);
	}

	if (egl_manager_x11) {
		egl_manager_x11->set_use_vsync(p_value);
	}
}

/**
 * @brief The core processing point of the current window. Any events that occur within the window will refer to this
 * point of the application.
 */
void DisplayManagerX11::process_events() {
	if (!display) {
		return; // No events to handle when the display is null
	}
	XEvent event;
	while (XPending(display)) {
		XNextEvent(display, &event);

		switch (event.type) {
			case KeymapNotify: {
				XRefreshKeyboardMapping(&event.xmapping);
			} break;
			case Expose: {
				XWindowAttributes window_attribs;
				XGetWindowAttributes(display, window->win, &window_attribs);
				if (gl_manager_x11) {
					gl_manager_x11->resize_viewport(window_attribs.width, window_attribs.height);
				}

				if (egl_manager_x11) {
					egl_manager_x11->resize_viewport(window_attribs.width, window_attribs.height);
				}

			} break;
			case ConfigureNotify: {
				_update_window(&event);
			} break;
			case ClientMessage: {
				if (event.xclient.data.l[0] == (int64_t)window->wm_close_atom) {
					window->notification_callback.fire(NOTIFICATION_WM_WINDOW_CLOSE, window->id);
					OS::get_singleton()->set_exit_code(0);
					Main::set_should_quit(true);
					return;
				}
			} break;
			case DestroyNotify: {
				window->notification_callback.fire(NOTIFICATION_WM_WINDOW_CLOSE, window->id);
				OS::get_singleton()->set_exit_code(0);
				Main::set_should_quit(true);
			} break;
			case PropertyNotify: {
				if (event.xproperty.atom == window->wm_state_atom) {
					_update_wm_properties();
				}
			} break;
			case KeyPress:
			case KeyRelease: {
				if (Input::get_singleton()->is_key_pressed(KEY_ALT) &&
					Input::get_singleton()->is_key_pressed(KEY_F4)) {
					// Want to quit, so send out a message
					XEvent ev;
					ev.xclient.data.l[0] = window->wm_close_atom;
					XSendEvent(display, window->win, false, ClientMessage, &ev);
				}

				bool is_pressed = (event.type == KeyPress ? true : false);
				KeySym keysym = 0;
				XLookupString(&event.xkey, nullptr, 0, &keysym, nullptr);
				Key k = translate_keycodes(keysym);

				Ref<InputEventKey> key_event;
				key_event.instantiate();
				key_event->key = k;
				key_event->pressed = is_pressed;

				Input::get_singleton()->parse_input_event(key_event);
			} break;
			case ButtonPress:
			case ButtonRelease: {
				bool is_pressed = (event.type == ButtonPress ? true : false);

				uint32_t p_button = event.xbutton.button;

				// Scroll event
				if (p_button == 4 || p_button == 5) {
					Ref<InputEventMouseScroll> ms_event;
					ms_event.instantiate();
					ms_event->scroll = p_button == 4 ? 1 : -1;

					if (event.type != ButtonPress) {
						ms_event->scroll = 0;
					}

					Input::get_singleton()->parse_input_event(ms_event);
				}

				MouseButton button = MOUSE_MAX;
				switch (event.xbutton.button) {
					case Button1:
						button = MOUSE_LBUTTON;
						break;
					case Button2:
						button = MOUSE_MBUTTON;
						break;
					case Button3:
						button = MOUSE_RBUTTON;
						break;
				}

				Ref<InputEventMouseButton> mb_event;
				mb_event.instantiate();
				mb_event->mb = button;
				mb_event->pressed = is_pressed;

				Input::get_singleton()->parse_input_event(mb_event);
			} break;
			case MotionNotify: {
				int x_position = event.xmotion.x;
				int y_position = event.xmotion.y;

				Vector2i pos(x_position, y_position);

				if (mouse_mode) {
					Vector2i c(window->size / 2);

					old_x = c.x;
					old_y = c.y;

					if (pos == c) {
						centre = c;
					} else {
						centre = pos;
						XWarpPointer(display, None, window->win, 0, 0, 0, 0, (int)c.x, (int)c.y);
					}
				}

				if (first_frame) {
					old_x = pos.x;
					old_y = pos.y;
					first_frame = false;
				}

				Ref<InputEventMouseMotion> mm_event;
				mm_event.instantiate();
				mm_event->absolute = pos;
				mm_event->relative = pos - Vector2i(old_x, old_y);

				Input::get_singleton()->parse_input_event(mm_event);

				old_x = pos.x;
				old_y = pos.y;

			} break;
			default: {
			} break;
		}
	}
}

/**
 * @brief Calls the OpenGL manager to change which framebuffer any draw calls are being sent to.
 */
void DisplayManagerX11::swap_buffers() {
	if (gl_manager_x11) {
		gl_manager_x11->swap_buffers();
	}

	if (egl_manager_x11) {
		egl_manager_x11->swap_buffers();
	}
}

Vector2i DisplayManagerX11::get_window_rect() const {
	ERR_COND_NULL_R(window, Vector2i());
	return window->size;
}

void DisplayManagerX11::set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) {
	if (p_id == window->id) {
		window->window_resize_callback = p_method;
	}
}

void DisplayManagerX11::toggle_mouse_mode(bool p_mode) {
	if (p_mode) {
		Cursor c;
		Pixmap p;
		XColor b;
		char data[] = {0, 0, 0, 0, 0, 0, 0, 0};
		b.red = b.green = b.blue = 0;
		p = XCreateBitmapFromData(display, window->win, data, 8, 8);
		c = XCreatePixmapCursor(display, p, p, &b, &b, 0, 0);
		XDefineCursor(display, window->win, c);
		XFreeCursor(display, c);
		XFreePixmap(display, p);

		Vector2i centre = get_window_rect();
		XWarpPointer(display, None, window->win, 0, 0, 0, 0, (int)centre.x / 2, (int)centre.y / 2);
	} else {
		Cursor c;
		c = XCreateFontCursor(display, XC_left_ptr);
		XDefineCursor(display, window->win, c);
		XFreeCursor(display, c);
	}

	mouse_mode = p_mode;
}

bool DisplayManagerX11::get_mouse_mode() const {
	return mouse_mode;
}

Key DisplayManagerX11::translate_keycodes(int p_code) {
	switch (p_code) {
		case XK_BackSpace:
			return KEY_BACKSPACE;
		case XK_Tab:
			return KEY_TAB;
		case XK_Clear:
			return KEY_CLEAR;
		case XK_KP_Enter:
			return KEY_ENTER;
		// TODO: Send an extra event for ANY shift key being pressed
		case XK_Shift_L:
			return KEY_LSHIFT;
		case XK_Shift_R:
			return KEY_RSHIFT;
		case XK_Control_L:
			return KEY_LCTRL;
		case XK_Control_R:
			return KEY_RCTRL;
		case XK_Alt_L:
			return KEY_LALT;
		case XK_Alt_R:
			return KEY_RALT;
		case XK_Caps_Lock:
			return KEY_CAPSLOCK;
		case XK_Escape:
			return KEY_ESCAPE;
		case XK_space:
			return KEY_SPACEBAR;
		case XK_Page_Up:
			return KEY_PAGEUP;
		case XK_Page_Down:
			return KEY_PAGEDOWN;
		case XK_End:
			return KEY_END;
		case XK_Home:
			return KEY_HOME;
		case XK_Left:
			return KEY_LEFT;
		case XK_Up:
			return KEY_UP;
		case XK_Right:
			return KEY_RIGHT;
		case XK_Down:
			return KEY_DOWN;
		case XK_Insert:
			return KEY_INSERT;
		case XK_Delete:
			return KEY_DELETE;
		case XK_0:
			return KEY_0;
		case XK_1:
			return KEY_1;
		case XK_2:
			return KEY_2;
		case XK_3:
			return KEY_3;
		case XK_4:
			return KEY_4;
		case XK_5:
			return KEY_5;
		case XK_6:
			return KEY_6;
		case XK_7:
			return KEY_7;
		case XK_8:
			return KEY_8;
		case XK_9:
			return KEY_9;
		case XK_A:
		case XK_a:
			return KEY_A;
		case XK_B:
		case XK_b:
			return KEY_B;
		case XK_C:
		case XK_c:
			return KEY_C;
		case XK_D:
		case XK_d:
			return KEY_D;
		case XK_E:
		case XK_e:
			return KEY_E;
		case XK_F:
		case XK_f:
			return KEY_F;
		case XK_G:
		case XK_g:
			return KEY_G;
		case XK_H:
		case XK_h:
			return KEY_H;
		case XK_I:
		case XK_i:
			return KEY_I;
		case XK_J:
		case XK_j:
			return KEY_J;
		case XK_K:
		case XK_k:
			return KEY_K;
		case XK_L:
		case XK_l:
			return KEY_L;
		case XK_M:
		case XK_m:
			return KEY_M;
		case XK_N:
		case XK_n:
			return KEY_N;
		case XK_O:
		case XK_o:
			return KEY_O;
		case XK_P:
		case XK_p:
			return KEY_P;
		case XK_Q:
		case XK_q:
			return KEY_Q;
		case XK_R:
		case XK_r:
			return KEY_R;
		case XK_S:
		case XK_s:
			return KEY_S;
		case XK_T:
		case XK_t:
			return KEY_T;
		case XK_U:
		case XK_u:
			return KEY_U;
		case XK_V:
		case XK_v:
			return KEY_V;
		case XK_W:
		case XK_w:
			return KEY_W;
		case XK_X:
		case XK_x:
			return KEY_X;
		case XK_Y:
		case XK_y:
			return KEY_Y;
		case XK_Z:
		case XK_z:
			return KEY_Z;
		case XK_KP_0:
			return KEY_NUMPAD0;
		case XK_KP_1:
			return KEY_NUMPAD1;
		case XK_KP_2:
			return KEY_NUMPAD2;
		case XK_KP_3:
			return KEY_NUMPAD3;
		case XK_KP_4:
			return KEY_NUMPAD4;
		case XK_KP_5:
			return KEY_NUMPAD5;
		case XK_KP_6:
			return KEY_NUMPAD6;
		case XK_KP_7:
			return KEY_NUMPAD7;
		case XK_KP_8:
			return KEY_NUMPAD8;
		case XK_KP_9:
			return KEY_NUMPAD9;
		case XK_KP_Multiply:
			return KEY_NUMPADMULTIPLY;
		case XK_KP_Add:
			return KEY_NUMPADADD;
		case XK_KP_Subtract:
			return KEY_NUMPADSUBTRACT;
		case XK_KP_Decimal:
			return KEY_NUMPADDECIMAL;
		case XK_KP_Divide:
			return KEY_NUMPADDIVIDE;
		case XK_F1:
			return KEY_F1;
		case XK_F2:
			return KEY_F2;
		case XK_F3:
			return KEY_F3;
		case XK_F4:
			return KEY_F4;
		case XK_F5:
			return KEY_F5;
		case XK_F6:
			return KEY_F6;
		case XK_F7:
			return KEY_F7;
		case XK_F8:
			return KEY_F8;
		case XK_F9:
			return KEY_F9;
		case XK_F10:
			return KEY_F10;
		case XK_F11:
			return KEY_F11;
		case XK_F12:
			return KEY_F12;
		case XK_F13:
			return KEY_F13;
		case XK_F14:
			return KEY_F14;
		case XK_F15:
			return KEY_F15;
		case XK_F16:
			return KEY_F16;
		case XK_F17:
			return KEY_F17;
		case XK_F18:
			return KEY_F18;
		case XK_F19:
			return KEY_F19;
		case XK_F20:
			return KEY_F20;
		case XK_F21:
			return KEY_F21;
		case XK_F22:
			return KEY_F22;
		case XK_F23:
			return KEY_F23;
		case XK_F24:
			return KEY_F24;
	}
	return KEY_MAX_KEYS; // Invalid keycode
}

/**
 * @brief The "shutdown" routine of the application. Stops all processing, clears out any windows that may still exist,
 * and closes the X11 display.
 */
void DisplayManagerX11::finalize() {
	if (window) {
		destroy_window(window->id);
	}

	if (gl_manager_x11) {
		gl_manager_x11->finalize();
	}

	if (egl_manager_x11) {
		egl_manager_x11->finalize();
	}

	if (display) {
		XCloseDisplay(display);
	}
}
DisplayManager *DisplayManagerX11::create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	return vnew(DisplayManagerX11(p_renderer, p_size, r_error));
}

/**
 * @brief Class constructor for the X11 display server. Opens an X11 conntect, gets the screen and its ID, as well as
 * calling the OpenGL manager to begin initialization.
 */
DisplayManagerX11::DisplayManagerX11(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	display = XOpenDisplay(nullptr);
	if (!display) {
		*r_error = ERR_CANT_CREATE;
		ERR_FAIL_MSG("Could not open a connection to the X11 display server.");
	}

	screen = DefaultScreenOfDisplay(display);
	screen_id = DefaultScreen(display);

	if (!screen) {
		*r_error = ERR_CANT_CREATE;
		ERR_FAIL_MSG("Could not find a suitable X11 screen.");
	}

	if (p_renderer == "opengl") {
		gl_manager_x11 = vnew(GLManagerX11);

		Error err = gl_manager_x11->initialize();
		if (err != OK) {
			*r_error = err;
			ERR_FAIL_MSG("Could not initialize the OpenGL manager for X11.");
		}

		RenderingServerGL::make_default(false);
	} else if (p_renderer == "opengl_es") {
		egl_manager_x11 = vnew(EGLManagerX11);

		Error err = egl_manager_x11->initialize();
		if (err != OK) {
			*r_error = err;
			ERR_FAIL_MSG("Could not initialize the EGL manager for X11.");
		}

		err = egl_manager_x11->open_display(display);
		if (err != OK) {
			*r_error = err;
			ERR_FAIL_MSG("Could not open a display on the EGL manager.");
		}

		RenderingServerGL::make_default(true);
	}

	if (create_window("Victoria Engine Window", 100, 100, p_size.x, p_size.y) != 0) {
		*r_error = ERR_CANT_CREATE;
		ERR_FAIL_MSG("Failed to create the root window for X11.");
	}
}

void DisplayManagerX11::register_x11_driver() {
	_create_funcs[create_func_count].func = create_func;
	_create_funcs[create_func_count].name = String("x11");
	create_func_count++;
}

/**
 * @brief Class destructor for the X11 display server. Frees and sets the OpenGL manager to null.
 */
DisplayManagerX11::~DisplayManagerX11() {
	if (gl_manager_x11) {
		vdelete(gl_manager_x11);
		gl_manager_x11 = nullptr;
	}

	if (egl_manager_x11) {
		vdelete(egl_manager_x11);
		egl_manager_x11 = nullptr;
	}
}

#endif // PLATFORM_LINUX
