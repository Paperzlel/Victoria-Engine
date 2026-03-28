#include "display_manager_wayland.h"
#if PLATFORM_LINUX && defined(WAYLAND_ENABLED)

#	include "core/error/error_macros.h"
#	include "core/io/input.h"
#	include "core/os/memory.h"
#	include "core/os/os.h"
#	include "drivers/opengl/rendering_server_gl.h"
#	include "main/main.h"

#	include <string.h>

// Override from the default `wl_array_for_each` as it doesn't work for C++ compilers.
// Workaround specifically from https://github.com/libretro/RetroArch/commit/8e638f435a37c46195aad1589ab024e443971d12
// (thanks RetroArch)
#	define WL_ARRAY_FOR_EACH(pos, array, type)                                                                       \
		for (pos = (type)(array)->data; (const char *)pos < ((const char *)(array)->data + (array)->size); (pos)++)

void DisplayManagerWayland::_on_registry_global(void *p_data,
												struct wl_registry *p_registry,
												uint32_t p_name,
												const char *p_interface,
												uint32_t p_version) {
	ClientData *data = (ClientData *)p_data;
	if (strcmp(p_interface, wl_compositor_interface.name) == 0) {
		data->compositor = (struct wl_compositor *)wl_registry_bind(p_registry, p_name, &wl_compositor_interface, 4);
	}

	if (strcmp(p_interface, wl_seat_interface.name) == 0) {
		data->seat = (struct wl_seat *)wl_registry_bind(p_registry, p_name, &wl_seat_interface, 7);
		wl_seat_add_listener(data->seat, &seat_listener, data);
	}

	if (strcmp(p_interface, xdg_wm_base_interface.name) == 0) {
		data->wm_base = (struct xdg_wm_base *)wl_registry_bind(p_registry, p_name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(data->wm_base, &wm_base_listener, data);
	}

	if (strcmp(p_interface, wp_cursor_shape_manager_v1_interface.name) == 0) {
		data->wp_cursor_shape = (struct wp_cursor_shape_manager_v1 *)
			wl_registry_bind(p_registry, p_name, &wp_cursor_shape_manager_v1_interface, 1);
	}

	if (strcmp(p_interface, zxdg_decoration_manager_v1_interface.name) == 0) {
		data->decor_manager = (struct zxdg_decoration_manager_v1 *)
			wl_registry_bind(p_registry, p_name, &zxdg_decoration_manager_v1_interface, 1);
	}
}

void DisplayManagerWayland::_on_registry_global_remove(void *p_data, struct wl_registry *p_registry, uint32_t p_name) {
}

void DisplayManagerWayland::_on_seat_capabilities_changed(void *p_data,
														  struct wl_seat *p_seat,
														  uint32_t p_capabilities) {
	ClientData *cd = (ClientData *)p_data;
	if (cd->seat != p_seat) {
		return;
	}

	bool has_pointer = (p_capabilities & WL_SEAT_CAPABILITY_POINTER);
	bool has_keyboard = (p_capabilities & WL_SEAT_CAPABILITY_KEYBOARD);

	if (has_pointer && !cd->pointer) {
		cd->pointer = wl_seat_get_pointer(cd->seat);
		wl_pointer_add_listener(cd->pointer, &pointer_listener, cd);
	} else if (!has_pointer && cd->pointer) {
		wl_pointer_release(cd->pointer);
		cd->pointer = nullptr;
	}

	if (has_keyboard && !cd->keyboard) {
		cd->keyboard = wl_seat_get_keyboard(cd->seat);
		// TODO: Bind keyboard listener
	} else if (!has_keyboard && cd->keyboard) {
		wl_keyboard_release(cd->keyboard);
		cd->keyboard = nullptr;
	}
}

void DisplayManagerWayland::_on_seat_name_changed(void *p_data, struct wl_seat *p_seat, const char *p_name) {
	ClientData *cd = (ClientData *)p_data;
	if (cd->seat != p_seat) {
		return;
	}
}

void DisplayManagerWayland::_on_pointer_enter(void *p_data,
											  struct wl_pointer *p_pointer,
											  uint32_t p_serial,
											  struct wl_surface *p_surface,
											  wl_fixed_t p_surface_x,
											  wl_fixed_t p_surface_y) {
	ClientData *cd = (ClientData *)p_data;
	DisplayManagerWayland *wl = static_cast<DisplayManagerWayland *>(DisplayManager::get_singleton());
	cd->active_window = wl->_get_window_id_from_surface(p_surface);
	if (cd->active_window == INVALID_WINDOW_ID) {
		ERR_WARN(vformat("Unknown surface %p entered by cursor", p_surface));
	}

	// Handle cursor data on our own.
	cd->wp_pointer_shape = wp_cursor_shape_manager_v1_get_pointer(cd->wp_cursor_shape, cd->pointer);
	wp_cursor_shape_device_v1_set_shape(cd->wp_pointer_shape, p_serial, WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT);
	wl->_window_push_event(cd->active_window, NOTIFICATION_WM_MOUSE_ENTER);
}

void DisplayManagerWayland::_on_pointer_leave(void *p_data,
											  struct wl_pointer *p_pointer,
											  uint32_t p_serial,
											  struct wl_surface *p_surface) {
	ClientData *cd = (ClientData *)p_data;
	DisplayManagerWayland *wl = static_cast<DisplayManagerWayland *>(DisplayManager::get_singleton());
	if (cd->active_window != INVALID_WINDOW_ID) {
		wl->_window_push_event(cd->active_window, NOTIFICATION_WM_MOUSE_EXIT);
		cd->active_window = INVALID_WINDOW_ID;

		wp_cursor_shape_device_v1_destroy(cd->wp_pointer_shape);
		cd->wp_pointer_shape = nullptr;
	} else {
		ERR_WARN("Wayland attempted to leave a window's surface without entering one.");
	}
}

void DisplayManagerWayland::_on_pointer_motion(void *p_data,
											   struct wl_pointer *p_pointer,
											   uint32_t p_time,
											   wl_fixed_t p_surface_x,
											   wl_fixed_t p_surface_y) {
	ClientData *cd = (ClientData *)p_data;
	DisplayManagerWayland *wl = static_cast<DisplayManagerWayland *>(DisplayManager::get_singleton());
	Vector2i npos(wl_fixed_to_int(p_surface_x), wl_fixed_to_int(p_surface_y));
	wl->_window_update_cursor_position(cd->active_window, npos);
}

void DisplayManagerWayland::_on_pointer_button(void *p_data,
											   struct wl_pointer *p_pointer,
											   uint32_t p_serial,
											   uint32_t p_time,
											   uint32_t p_button,
											   uint32_t p_state) {}

void DisplayManagerWayland::_on_pointer_axis(void *p_data,
											 struct wl_pointer *p_pointer,
											 uint32_t p_time,
											 uint32_t p_axis,
											 wl_fixed_t p_fixed) {}

void DisplayManagerWayland::_on_pointer_frame(void *p_data, struct wl_pointer *p_pointer) {
	// Pushes a pointer frame, or in other words tells our client that it
	// needs to update the cursor's position.
	ClientData *cd = (ClientData *)p_data;
	if (cd->active_window == INVALID_WINDOW_ID) {
		return;
	}

	cd->frame_recieved = true;
}

void DisplayManagerWayland::_on_xdg_surface_configure(void *p_data, struct xdg_surface *p_surface, uint32_t p_serial) {
	// Acknowledge, but do nothing. In most cases EGL will look after window data.
	xdg_surface_ack_configure(p_surface, p_serial);
}

void DisplayManagerWayland::_on_xdg_toplevel_configure(void *p_data,
													   struct xdg_toplevel *p_toplevel,
													   int32_t width,
													   int32_t height,
													   struct wl_array *p_states) {
	WindowData *wd = (WindowData *)p_data;

	// Reset window flags on configure to false, they wil be updated once done.
	wd->resizing = false;
	wd->maximised = false;

	uint32_t *state = nullptr;
	WL_ARRAY_FOR_EACH(state, p_states, uint32_t *) {
		switch (*(uint32_t *)state) {
			case XDG_TOPLEVEL_STATE_MAXIMIZED: {
				wd->maximised = true;
			} break;
			case XDG_TOPLEVEL_STATE_RESIZING: {
				wd->resizing = true;
			} break;
		}
	}

	// Size is passed through, update size.
	if (width != 0 || height != 0) {
		wd->size.x = width;
		wd->size.y = height;
	}

	// Send an update to resize the window on focus enter events. We don't
	// yet handle those properly elsewhere.
	if (!wd->maximised && !wd->resizing) {
		wd->cached_size = Vector2i(width, height);
		wd->is_size_dirty = true;
	}

	// TODO: Update window size if not equal to zero (means the compositor wants to configure window size)
}

void DisplayManagerWayland::_on_xdg_toplevel_close(void *p_data, struct xdg_toplevel *p_toplevel) {
	OS::get_singleton()->set_exit_code(0);
	Main::set_should_quit(true);
}

void DisplayManagerWayland::_on_xdg_wm_base_ping(void *p_data, struct xdg_wm_base *p_base, uint32_t p_serial) {
	xdg_wm_base_pong(p_base, p_serial);
}

void DisplayManagerWayland::_on_zxdg_decoration_manager_configure(void *p_data,
																  struct zxdg_toplevel_decoration_v1 *p_decor,
																  uint32_t p_mode) {
	if (p_mode != ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE) {
		ERR_WARN("Victoria Engine does not support client-side decorations!");
	}
}

uint8_t DisplayManagerWayland::_get_window_id_from_surface(struct wl_surface *p_surface) {
	if (p_surface == wd->wl_surface) {
		return 0;
	}

	return INVALID_WINDOW_ID;
}

void DisplayManagerWayland::_window_update_cursor_position(uint8_t p_window, const Vector2i &p_position) {
	if (p_window != 0 || p_window == INVALID_WINDOW_ID) {
		return;
	}

	// To accumulate pointer events, we simply update the current position every frame.
	wd->cursor_pos = p_position;
}

void DisplayManagerWayland::_window_push_event(uint8_t p_window, WindowNotification p_notification) {
	if (p_window != 0 || p_window == INVALID_WINDOW_ID) {
		return;
	}
	// Do nothing for now.
}

DisplayManager *DisplayManagerWayland::create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	return vnew(DisplayManagerWayland(p_renderer, p_size, r_error));
}

void DisplayManagerWayland::register_wayland_driver() {
	_create_funcs[create_func_count].func = create_func;
	_create_funcs[create_func_count].name = String("wayland");
	create_func_count++;
}

uint8_t DisplayManagerWayland::create_window(const String &p_name,
											 uint16_t x,
											 uint16_t y,
											 uint16_t width,
											 uint16_t height,
											 WindowFlags p_flags) {
	wd = vnew(WindowData);
	wd->wl_surface = wl_compositor_create_surface(client_data->compositor);

	wd->xdg_surface = xdg_wm_base_get_xdg_surface(client_data->wm_base, wd->wl_surface);
	xdg_surface_add_listener(wd->xdg_surface, &surface_listener, wd);
	wd->toplevel = xdg_surface_get_toplevel(wd->xdg_surface);
	xdg_toplevel_add_listener(wd->toplevel, &toplevel_listener, wd);

	if (client_data->decor_manager) {
		wd->toplevel_decoration =
			zxdg_decoration_manager_v1_get_toplevel_decoration(client_data->decor_manager, wd->toplevel);
		zxdg_toplevel_decoration_v1_add_listener(wd->toplevel_decoration, &toplevel_decor_listener, wd);

		zxdg_toplevel_decoration_v1_set_mode(wd->toplevel_decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
	}

	xdg_toplevel_set_title(wd->toplevel, p_name.get_data());
	xdg_surface_set_window_geometry(wd->xdg_surface, 0, 0, width, height);
	wl_surface_commit(wd->wl_surface);

	wl_display_roundtrip(display);

	wd->size = Vector2i(width, height);
	// Store cached size prior to updates.
	wd->cached_size = wd->size;
	wd->position = Vector2i(x, y);
	wd->notification_callback.connect(static_callable_mp(_notification_callback), false);

	if (egl_manager_wl) {
		wd->egl_window = wl_egl_window_create(wd->wl_surface, width, height);
		uint8_t id = egl_manager_wl->create_window(display, wd->egl_window);
		ERR_FAIL_COND_MSG_R(id == INVALID_WINDOW_ID, "Failed to create Wayland EGL window.", INVALID_WINDOW_ID);
		wd->id = id;
	}

	return wd->id;
}

void DisplayManagerWayland::destroy_window(uint8_t p_id) {
	if (p_id == INVALID_WINDOW_ID) {
		return; // Dont handle invalid windows
	}

	if (wd->toplevel_decoration) {
		zxdg_toplevel_decoration_v1_destroy(wd->toplevel_decoration);
	}

	if (wd->toplevel) {
		xdg_toplevel_destroy(wd->toplevel);
	}

	if (wd->xdg_surface) {
		xdg_surface_destroy(wd->xdg_surface);
	}

	if (wd->wl_surface) {
		wl_surface_destroy(wd->wl_surface);
	}

	vdelete(wd);
	wd = nullptr;
}

void DisplayManagerWayland::set_use_vsync(bool p_value) {
	if (egl_manager_wl) {
		egl_manager_wl->set_use_vsync(p_value);
	}
}

Vector2i DisplayManagerWayland::get_window_rect() const {
	ERR_FAIL_COND_R(wd == nullptr, Vector2i());
	return wd->size;
}

void DisplayManagerWayland::set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) {
	if (p_id == wd->id) {
		wd->resize_callback = p_method;
	}
}

void DisplayManagerWayland::toggle_mouse_mode(bool p_mode) {}

bool DisplayManagerWayland::get_mouse_mode() const {
	return false;
}

void DisplayManagerWayland::process_events() {
	// Dispatch events that have built up.
	int dispatched = wl_display_dispatch_pending(display);
	if (dispatched == -1) {
		// error on dispatch!
	}

	// Force resize events if certain parameters are called.
	bool force_resize_event = false;
	if (wd->is_size_dirty) {
		wd->size = wd->cached_size;
		wd->is_size_dirty = false;
		force_resize_event = true;
	}

	if (wd->resizing || wd->maximised || force_resize_event) {
		if (wd->wl_surface) {
			xdg_surface_set_window_geometry(wd->xdg_surface, 0, 0, wd->size.x, wd->size.y);
		}

		if (egl_manager_wl) {
			wl_egl_window_resize(wd->egl_window, wd->size.x, wd->size.y, 0, 0);
		}

		if (wd->resize_callback.is_valid()) {
			wd->resize_callback.call(wd->id);
		}
	}

	if (client_data->frame_recieved) {
		// TODO: implement mouse mode focus

		if (wd->is_first_frame) {
			wd->last_cursor_pos = wd->cursor_pos;
			wd->is_first_frame = false;
		}

		Ref<InputEventMouseMotion> mm;
		mm.instantiate();

		mm->absolute = wd->cursor_pos;
		mm->relative = wd->cursor_pos - wd->last_cursor_pos;
		Input::get_singleton()->parse_input_event(mm);
		wd->last_cursor_pos = wd->cursor_pos;
		client_data->frame_recieved = false;
	}
}

void DisplayManagerWayland::swap_buffers() {
	if (egl_manager_wl) {
		egl_manager_wl->swap_buffers();
	}
}

void DisplayManagerWayland::finalize() {
	if (wd) {
		destroy_window(wd->id);
	}

	if (egl_manager_wl) {
		egl_manager_wl->finalize();
	}

	if (display) {
		wl_display_disconnect(display);
	}
}

DisplayManagerWayland::DisplayManagerWayland(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	display = wl_display_connect(nullptr);
	if (!display) {
		*r_error = ERR_CANT_CREATE;
		ERR_FAIL_MSG("Failed to connect the display to the Wayland compositor.");
	}

	client_data = vnew(ClientData);

	struct wl_registry *reg = wl_display_get_registry(display);
	if (!reg) {
		*r_error = ERR_CANT_CREATE;
		ERR_FAIL_MSG("Failed to obtain the Wayland registry.");
	}

	wl_registry_add_listener(reg, &global_listener, client_data);
	wl_display_roundtrip(display);

	// We currently run GL and GLES under the same configuration. In the future, we'll need to change what
	// in case GLES is actually supported.
	if (p_renderer == "opengl" || p_renderer == "opengl_es") {
		egl_manager_wl = vnew(EGLManagerWayland);

		Error err = egl_manager_wl->initialize();
		if (err != OK) {
			*r_error = err;
			ERR_FAIL_MSG("Could not initialize the EGL manager for Wayland.");
		}

		err = egl_manager_wl->open_display(display);
		if (err != OK) {
			*r_error = err;
			ERR_FAIL_MSG("Could not open the display for the EGL manager.");
		}

		RenderingServerGL::make_default(false);
	}

	if (create_window("Victoria Engine Window", 100, 100, p_size.x, p_size.y) != 0) {
		*r_error = ERR_CANT_CREATE;
		ERR_FAIL_MSG("Failed to create the root window under Wayland.");
	}
}

DisplayManagerWayland::~DisplayManagerWayland() {
	vdelete(client_data);

	if (egl_manager_wl) {
		vdelete(egl_manager_wl);
	}
}

#endif // PLATFORM_LINUX && WAYLAND_ENABLED
