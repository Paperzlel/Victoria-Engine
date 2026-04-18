#include "display_manager_wayland.h"
#if defined(PLATFORM_LINUX) && defined(WAYLAND_ENABLED)

#	include "core/error/error_macros.h"
#	include "core/io/input.h"
#	include "core/os/memory.h"
#	include "core/os/os.h"

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
	RegistryData *rd = (RegistryData *)p_data;

	if (rd->registry_names.has(p_name)) {
		ERR_WARN("Duplicated Wayland global!");
	}
	rd->registry_names.push_back(p_name);

	if (strcmp(p_interface, wl_compositor_interface.name) == 0) {
		rd->compositor = (struct wl_compositor *)wl_registry_bind(p_registry, p_name, &wl_compositor_interface, 4);
		rd->compositor_name = p_name;
	}

	if (strcmp(p_interface, wl_seat_interface.name) == 0) {
		rd->seat = (struct wl_seat *)wl_registry_bind(p_registry, p_name, &wl_seat_interface, 7);
		rd->seat_name = p_name;

		SeatData *sd = vnew(SeatData);
		sd->seat = rd->seat;
		sd->seat_name = p_name;

		sd->rd = rd;
		sd->wayland = rd->wayland;

		wl_seat_add_listener(rd->seat, &seat_listener, sd);
	}

	if (strcmp(p_interface, xdg_wm_base_interface.name) == 0) {
		rd->wm_base = (struct xdg_wm_base *)wl_registry_bind(p_registry, p_name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(rd->wm_base, &wm_base_listener, rd);
		rd->wm_base_name = p_name;
	}

	if (strcmp(p_interface, wp_cursor_shape_manager_v1_interface.name) == 0) {
		rd->wp_cursor_shape = (struct wp_cursor_shape_manager_v1 *)
			wl_registry_bind(p_registry, p_name, &wp_cursor_shape_manager_v1_interface, 1);
		rd->wp_cursor_shape_name = p_name;
	}

	if (strcmp(p_interface, zxdg_decoration_manager_v1_interface.name) == 0) {
		rd->zxdg_decor_manager = (struct zxdg_decoration_manager_v1 *)
			wl_registry_bind(p_registry, p_name, &zxdg_decoration_manager_v1_interface, 1);
		rd->zxdg_decor_manager_name = p_name;
	}

	if (strcmp(p_interface, zwp_relative_pointer_manager_v1_interface.name) == 0) {
		rd->zwp_relative_pointer = (struct zwp_relative_pointer_manager_v1 *)
			wl_registry_bind(p_registry, p_name, &zwp_relative_pointer_manager_v1_interface, 1);
		rd->zwp_relative_pointer_name = p_name;
	}

	if (strcmp(p_interface, zwp_pointer_constraints_v1_interface.name) == 0) {
		rd->zwp_pointer_constraints = (struct zwp_pointer_constraints_v1 *)
			wl_registry_bind(p_registry, p_name, &zwp_pointer_constraints_v1_interface, 1);
		rd->zwp_pointer_constraints_name = p_name;
	}

	if (strcmp(p_interface, wp_pointer_warp_v1_interface.name) == 0) {
		rd->wp_pointer_warp =
			(struct wp_pointer_warp_v1 *)wl_registry_bind(p_registry, p_name, &wp_pointer_warp_v1_interface, 1);
		rd->wp_pointer_warp_name = p_name;
	}
}

void DisplayManagerWayland::_on_registry_global_remove(void *p_data, struct wl_registry *p_registry, uint32_t p_name) {
	RegistryData *rd = (RegistryData *)p_data;
	if (p_name == rd->compositor_name) {
		if (rd->compositor) {
			wl_compositor_destroy(rd->compositor);
			rd->compositor = nullptr;
		}

		rd->compositor_name = 0;
		return;
	}

	if (p_name == rd->seat_name) {
		SeatData *sd = _seat_get_seat_data(rd->seat);
		ERR_COND_NULL(sd);

		if (rd->seat) {
			wl_seat_destroy(rd->seat);
			rd->seat = nullptr;
		}

		if (sd->zwp_locked_pointer) {
			zwp_locked_pointer_v1_destroy(sd->zwp_locked_pointer);
			sd->zwp_locked_pointer = nullptr;
		}

		vdelete(sd);
		rd->seat_name = 0;
		return;
	}

	if (p_name == rd->wm_base_name) {
		if (rd->wm_base) {
			xdg_wm_base_destroy(rd->wm_base);
			rd->wm_base = nullptr;
		}

		rd->wm_base_name = 0;
		return;
	}

	if (p_name == rd->wp_cursor_shape_name) {
		if (rd->wp_cursor_shape) {
			wp_cursor_shape_manager_v1_destroy(rd->wp_cursor_shape);
			rd->wp_cursor_shape = nullptr;
		}

		rd->wp_cursor_shape_name = 0;
		return;
	}

	if (p_name == rd->zxdg_decor_manager_name) {
		if (rd->zxdg_decor_manager) {
			zxdg_decoration_manager_v1_destroy(rd->zxdg_decor_manager);
			rd->zxdg_decor_manager = nullptr;
		}

		rd->zxdg_decor_manager_name = 0;
		return;
	}

	if (p_name == rd->zwp_pointer_constraints_name) {
		if (rd->zwp_pointer_constraints) {
			zwp_pointer_constraints_v1_destroy(rd->zwp_pointer_constraints);
			rd->zwp_pointer_constraints = nullptr;
		}

		rd->zwp_pointer_constraints_name = 0;
		return;
	}

	if (p_name == rd->wp_pointer_warp_name) {
		if (rd->wp_pointer_warp) {
			wp_pointer_warp_v1_destroy(rd->wp_pointer_warp);
			rd->wp_pointer_warp = nullptr;
		}

		rd->wp_pointer_warp_name = 0;
		return;
	}
}

void DisplayManagerWayland::_on_seat_capabilities_changed(void *p_data,
														  struct wl_seat *p_seat,
														  uint32_t p_capabilities) {
	SeatData *sd = (SeatData *)p_data;
	if (sd->rd->seat != p_seat) {
		return;
	}

	if (p_capabilities & WL_SEAT_CAPABILITY_POINTER) {
		if (!sd->pointer) {
			sd->pointer = wl_seat_get_pointer(sd->rd->seat);
			wl_pointer_add_listener(sd->pointer, &pointer_listener, sd);
		}

		if (!sd->zwp_relative_pointer) {
			sd->zwp_relative_pointer =
				zwp_relative_pointer_manager_v1_get_relative_pointer(sd->rd->zwp_relative_pointer, sd->pointer);
			zwp_relative_pointer_v1_add_listener(sd->zwp_relative_pointer, &zwp_relative_pointer_listener, sd);
		}

		if (!sd->wp_cursor_shape) {
			sd->wp_cursor_shape = wp_cursor_shape_manager_v1_get_pointer(sd->rd->wp_cursor_shape, sd->pointer);
		}
	} else {
		if (sd->pointer) {
			wl_pointer_destroy(sd->pointer);
			sd->pointer = nullptr;
		}

		if (sd->zwp_relative_pointer) {
			zwp_relative_pointer_v1_destroy(sd->zwp_relative_pointer);
			sd->zwp_relative_pointer = nullptr;
		}

		if (sd->wp_cursor_shape) {
			wp_cursor_shape_device_v1_destroy(sd->wp_cursor_shape);
			sd->wp_cursor_shape = nullptr;
		}
	}

	if (p_capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
		if (!sd->keyboard) {
			sd->keyboard = wl_seat_get_keyboard(sd->rd->seat);
			// TODO: Bind keyboard listener
		}
	} else {
		if (sd->keyboard) {
			wl_keyboard_destroy(sd->keyboard);
			sd->keyboard = nullptr;
		}
	}
}

void DisplayManagerWayland::_on_seat_name_changed(void *p_data, struct wl_seat *p_seat, const char *p_name) {
	SeatData *sd = (SeatData *)p_data;
	if (sd->rd->seat != p_seat) {
		return;
	}
}

void DisplayManagerWayland::_on_pointer_enter(void *p_data,
											  struct wl_pointer *p_pointer,
											  uint32_t p_serial,
											  struct wl_surface *p_surface,
											  wl_fixed_t p_surface_x,
											  wl_fixed_t p_surface_y) {
	SeatData *sd = (SeatData *)p_data;
	ERR_COND_NULL(sd);
	sd->active_window = sd->wayland->_get_window_id_from_surface(p_surface);
	if (sd->active_window == INVALID_WINDOW_ID) {
		ERR_WARN(vformat("Unknown surface %p entered by cursor", p_surface));
	}

	// Handle cursor data on our own.
	sd->active_serial_id = p_serial;
	sd->wayland->toggle_mouse_mode(sd->wayland->mouse_mode);
	sd->wayland->_window_push_event(sd->active_window, NOTIFICATION_WM_MOUSE_ENTER);
}

void DisplayManagerWayland::_on_pointer_leave(void *p_data,
											  struct wl_pointer *p_pointer,
											  uint32_t p_serial,
											  struct wl_surface *p_surface) {
	SeatData *sd = (SeatData *)p_data;
	ERR_COND_NULL(sd);
	if (sd->active_window != INVALID_WINDOW_ID) {
		sd->wayland->_window_push_event(sd->active_window, NOTIFICATION_WM_MOUSE_EXIT);
		sd->active_window = INVALID_WINDOW_ID;
		sd->active_serial_id = 0;
	} else {
		ERR_WARN("Wayland attempted to leave a window's surface without entering one.");
	}
}

void DisplayManagerWayland::_on_pointer_motion(void *p_data,
											   struct wl_pointer *p_pointer,
											   uint32_t p_time,
											   wl_fixed_t p_surface_x,
											   wl_fixed_t p_surface_y) {
	SeatData *sd = (SeatData *)p_data;

	PointerData &pd = sd->pointer_data_write;

	pd.position.x = wl_fixed_to_int(p_surface_x);
	pd.position.y = wl_fixed_to_int(p_surface_y);
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
	SeatData *sd = (SeatData *)p_data;
	if (sd->active_window == INVALID_WINDOW_ID) {
		return;
	}

	sd->frame_recieved = true;
}

void DisplayManagerWayland::_on_pointer_axis_source(void *p_data,
													struct wl_pointer *p_pointer,
													uint32_t p_axis_source) {}

void DisplayManagerWayland::_on_pointer_axis_stop(void *p_data,
												  struct wl_pointer *p_pointer,
												  uint32_t p_time,
												  uint32_t p_axis) {}

void DisplayManagerWayland::_on_pointer_axis_discrete(void *p_data,
													  struct wl_pointer *p_pointer,
													  uint32_t p_axis,
													  int p_discrete) {}

void DisplayManagerWayland::_on_pointer_axis_value120(void *p_data,
													  struct wl_pointer *p_pointer,
													  uint32_t p_axis,
													  int p_value120) {}

void DisplayManagerWayland::_on_pointer_axis_relative_direction(void *p_data,
																struct wl_pointer *p_pointer,
																uint32_t p_axis,
																uint32_t p_direction) {}

void DisplayManagerWayland::_on_relative_pointer_relative_motion(void *p_data,
																 struct zwp_relative_pointer_v1 *p_relative_pointer,
																 uint32_t p_utime_high,
																 uint32_t p_utime_low,
																 wl_fixed_t p_dx,
																 wl_fixed_t p_dy,
																 wl_fixed_t p_dx_unaccel,
																 wl_fixed_t p_dy_unaccel) {
	SeatData *sd = (SeatData *)p_data;
	ERR_COND_NULL(sd);

	PointerData &pd = sd->pointer_data_write;
	pd.relative_position.x = wl_fixed_to_int(p_dx);
	pd.relative_position.y = wl_fixed_to_int(p_dy);
	pd.relative_motion_time = p_utime_low;
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
	OS::get_singleton()->set_should_quit(true);
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

void DisplayManagerWayland::_on_zwp_locked_pointer_lock(void *p_data, struct zwp_locked_pointer_v1 *p_locked_pointer) {
	SeatData *sd = (SeatData *)p_data;
	ERR_COND_NULL(sd);

	// Hide pointer from focus
	wl_pointer_set_cursor(sd->pointer, sd->active_serial_id, nullptr, 0, 0);

	PointerData &pd = sd->pointer_data_read;
	zwp_locked_pointer_v1_set_cursor_position_hint(sd->zwp_locked_pointer,
												   wl_fixed_from_int(pd.position.x),
												   wl_fixed_from_int(pd.position.y));
}

void DisplayManagerWayland::_on_zwp_locked_pointer_unlock(void *p_data,
														  struct zwp_locked_pointer_v1 *p_locked_pointer) {}

DisplayManagerWayland::SeatData *DisplayManagerWayland::_seat_get_seat_data(struct wl_seat *p_seat) {
	return (SeatData *)wl_seat_get_user_data(p_seat);
}

uint8_t DisplayManagerWayland::_get_window_id_from_surface(struct wl_surface *p_surface) {
	if (p_surface == wd->wl_surface) {
		return 0;
	}

	return INVALID_WINDOW_ID;
}

DisplayManagerWayland::WindowData *DisplayManagerWayland::_get_window_data_from_id(uint8_t p_window) {
	if (p_window == INVALID_WINDOW_ID || p_window != wd->id) {
		return nullptr;
	}

	return wd;
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
	wd->wl_surface = wl_compositor_create_surface(rd->compositor);

	wd->xdg_surface = xdg_wm_base_get_xdg_surface(rd->wm_base, wd->wl_surface);
	xdg_surface_add_listener(wd->xdg_surface, &surface_listener, wd);
	wd->toplevel = xdg_surface_get_toplevel(wd->xdg_surface);
	xdg_toplevel_add_listener(wd->toplevel, &toplevel_listener, wd);

	if (rd->zxdg_decor_manager) {
		wd->toplevel_decoration =
			zxdg_decoration_manager_v1_get_toplevel_decoration(rd->zxdg_decor_manager, wd->toplevel);
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

	if (wd->egl_window) {
		wl_egl_window_destroy(wd->egl_window);
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

	wl_display_roundtrip(display);

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

void DisplayManagerWayland::toggle_mouse_mode(bool p_mode) {
	Vector2i centre = wd->size / 2;
	if (p_mode) {
		// Lock pointer while the object is active.
		if (!sd->zwp_locked_pointer) {
			sd->zwp_locked_pointer =
				zwp_pointer_constraints_v1_lock_pointer(rd->zwp_pointer_constraints,
														wd->wl_surface,
														sd->pointer,
														nullptr,
														ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT);
			zwp_locked_pointer_v1_add_listener(sd->zwp_locked_pointer, &zwp_locked_pointer_listener, sd);
		}

		// (Optional) Warp pointer to centre. Only available on a handful of compositors.
		if (rd->wp_pointer_warp) {
			wp_pointer_warp_v1_warp_pointer(rd->wp_pointer_warp,
											wd->wl_surface,
											sd->pointer,
											wl_fixed_from_int(centre.x),
											wl_fixed_from_int(centre.y),
											sd->active_serial_id);
		}
	} else {
		// Destroy lock
		if (sd->zwp_locked_pointer) {
			zwp_locked_pointer_v1_destroy(sd->zwp_locked_pointer);
			sd->zwp_locked_pointer = nullptr;
		}

		// Show device. This event is called whenever a pointer enters the surface, so the lock may not exist
		// yet and thus we call it ourselves
		if (sd->wp_cursor_shape) {
			wp_cursor_shape_device_v1_set_shape(sd->wp_cursor_shape,
												sd->active_serial_id,
												WP_CURSOR_SHAPE_DEVICE_V1_SHAPE_DEFAULT);
		}
	}
	mouse_mode = p_mode;
}

bool DisplayManagerWayland::get_mouse_mode() const {
	return mouse_mode;
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

	if (sd->frame_recieved) {
		PointerData &old_pd = sd->pointer_data_read;
		PointerData &pd = sd->pointer_data_write;

		Ref<InputEventMouseMotion> mm;
		mm.instantiate();

		mm->absolute = pd.position;
		if (pd.relative_motion_time != old_pd.relative_motion_time) {
			mm->relative = pd.relative_position;
		} else {
			// Relative pointer motion is SOMETIMES not sent to the user, so do a quick calculation for it
			// beforehand.
			mm->relative = pd.position - old_pd.position;
		}
		Input::get_singleton()->parse_input_event(mm);
		old_pd = pd;
		sd->frame_recieved = false;
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

	Vector<uint32_t> global_names;
	for (uint32_t name : rd->registry_names) {
		global_names.push_back(name);
	}

	for (uint32_t name : global_names) {
		_on_registry_global_remove(wl_registry_get_user_data(registry), registry, name);
	}

	if (registry) {
		wl_registry_destroy(registry);
	}

	wl_display_roundtrip(display);

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

	rd = vnew(RegistryData);
	rd->wayland = this;

	registry = wl_display_get_registry(display);
	if (!registry) {
		*r_error = ERR_CANT_CREATE;
		ERR_FAIL_MSG("Failed to obtain the Wayland registry.");
	}

	wl_registry_add_listener(registry, &global_listener, rd);
	wl_display_roundtrip(display);

	sd = _seat_get_seat_data(rd->seat);

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

		OS::get_singleton()->set_rendering_driver("opengl");
		OS::get_singleton()->set_gles_over_gl(false);
	}

	if (create_window("Victoria Engine Window", 100, 100, p_size.x, p_size.y) != 0) {
		*r_error = ERR_CANT_CREATE;
		ERR_FAIL_MSG("Failed to create the root window under Wayland.");
	}
}

DisplayManagerWayland::~DisplayManagerWayland() {
	vdelete(rd);

	if (egl_manager_wl) {
		vdelete(egl_manager_wl);
	}
}

#endif // defined(PLATFORM_LINUX) && defined(WAYLAND_ENABLED)
