#include "display_manager_x11.h"
#if defined(PLATFORM_LINUX) && defined(X11_ENABLED)

#	include "keyboard_remapping_x11.h"

#	include "core/input/input.h"
#	include "core/os/os.h"

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

		// Free states (alloc'd by X11)
		free(states);
	}

	Atom minimized_atom = XInternAtom(display, "_NET_WM_STATE_HIDDEN", false);
	Atom maximised_x = XInternAtom(display, "_NET_WM_STATE_HORZ", false);
	Atom maximised_y = XInternAtom(display, "_NET_WM_STATE_MAXIMISED_VERT", false);

	window->maximised = local_states.has(maximised_x) && local_states.has(maximised_y);
	window->minimized = local_states.has(minimized_atom);

	OS::get_singleton()->set_is_suspended(window->minimized);
}

void DisplayManagerX11::_dispatch_input_event_s(const Ref<InputEvent> &p_event) {
	static_cast<DisplayManagerX11 *>(DisplayManager::get_singleton())->_dispatch_input_event(p_event);
}

void DisplayManagerX11::_dispatch_input_event(const Ref<InputEvent> &p_event) {
	WindowData *wd = window;
	if (wd && wd->input_event_callback.is_valid()) {
		wd->input_event_callback.call(p_event);
	}
}

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
		if (id == INVALID_WINDOW_ID) {
			vdelete(gl_manager_x11);
			OS::get_singleton()->print_error(__FILE__, FUNCTION_STR, __LINE__, "GLWindow was unable to be created.");
			return INVALID_WINDOW_ID;
		}
		win_data->id = id;
	}

	if (egl_manager_x11) {
		uint8_t id = egl_manager_x11->create_window(display, &win_data->win);
		if (id == INVALID_WINDOW_ID) {
			vdelete(egl_manager_x11);
			OS::get_singleton()->print_error(__FILE__, FUNCTION_STR, __LINE__, "EGLWindow was unable to be created,");
			return INVALID_WINDOW_ID;
		}

		win_data->id = id;
	}

	window = win_data;
	return window->id;
}

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
					destroy_window(window->id);
					OS::get_singleton()->set_exit_code(0);
					OS::get_singleton()->set_should_quit(true);
					return;
				}
			} break;
			case DestroyNotify: {
				destroy_window(window->id);
				OS::get_singleton()->set_exit_code(0);
				OS::get_singleton()->set_should_quit(true);
			} break;
			case PropertyNotify: {
				if (event.xproperty.atom == window->wm_state_atom) {
					_update_wm_properties();
				}
			} break;
			case KeyPress:
			case KeyRelease: {
				if (Input::get_singleton()->is_key_pressed(Key::ALT) &&
					Input::get_singleton()->is_key_pressed(Key::F4)) {
					// Want to quit, so send out a message
					XEvent ev;
					ev.xclient.data.l[0] = window->wm_close_atom;
					XSendEvent(display, window->win, false, ClientMessage, &ev);
				}

				bool is_pressed = (event.type == KeyPress ? true : false);
				KeySym keysym = 0;
				XLookupString(&event.xkey, nullptr, 0, &keysym, nullptr);
				KeyCode kc = XKeysymToKeycode(display, keysym);
				Key k = KeyboardRemappingX11::get_key_from_keycode(kc);

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
					break;
				}

				InputEnums::MouseButton button = InputEnums::MOUSE_MAX;
				switch (event.xbutton.button) {
					case Button1:
						button = InputEnums::MOUSE_LBUTTON;
						break;
					case Button2:
						button = InputEnums::MOUSE_MBUTTON;
						break;
					case Button3:
						button = InputEnums::MOUSE_RBUTTON;
						break;
				}

				Ref<InputEventMouseButton> mb_event;
				mb_event.instantiate();
				mb_event->button = button;
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

void DisplayManagerX11::swap_buffers() {
	if (gl_manager_x11) {
		gl_manager_x11->swap_buffers();
	}

	if (egl_manager_x11) {
		egl_manager_x11->swap_buffers();
	}
}

Vector2i DisplayManagerX11::get_window_size(uint8_t p_id) const {
	ERR_COND_NULL_R(window, Vector2i());
	ERR_FAIL_COND_R(p_id != 0, Vector2i());
	return window->size;
}

void DisplayManagerX11::set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) {
	if (p_id == window->id) {
		window->window_resize_callback = p_method;
	}
}

void DisplayManagerX11::set_input_event_dispatch_callback(const CallableMethod &p_method, uint8_t p_id) {
	if (p_id == window->id) {
		window->input_event_callback = p_method;
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

		Vector2i centre = get_window_size(window->id);
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

DisplayManagerX11::DisplayManagerX11(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	KeyboardRemappingX11::initialize();

	Input::get_singleton()->set_input_event_callback(&DisplayManagerX11::_dispatch_input_event_s);

	display = XOpenDisplay(nullptr);
	if (!display) {
		*r_error = ERR_CANT_CONNECT;
		ERR_FAIL_MSG("Could not open a connection to the X11 display server.");
	}

	screen = DefaultScreenOfDisplay(display);
	screen_id = DefaultScreen(display);

	if (!screen) {
		*r_error = ERR_CANT_LOAD;
		ERR_FAIL_MSG("Could not find a suitable X11 screen.");
	}

	if (p_renderer == "opengl") {
		gl_manager_x11 = vnew(GLManagerX11);

		Error err = gl_manager_x11->initialize();
		if (err != OK) {
			*r_error = err;
			ERR_FAIL_MSG("Could not initialize the OpenGL manager for X11.");
		}

		OS::get_singleton()->set_gles_over_gl(false);
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

		OS::get_singleton()->set_gles_over_gl(true);
	}

	if (p_renderer == "opengl" || p_renderer == "opengl_es") {
		OS::get_singleton()->set_rendering_driver("opengl");
	}

	if (create_window("Victoria Engine Window", 100, 100, p_size.x, p_size.y) != 0) {
		*r_error = ERR_CANT_CREATE;
		ERR_FAIL_MSG("Failed to create the root window for X11.");
	}
}

void DisplayManagerX11::register_x11_driver() {
	_create_funcs[create_func_count].func = create_func;
	_create_funcs[create_func_count].name = "x11";
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

#endif // defined(PLATFORM_LINUX) && defined(X11_ENABLED)
