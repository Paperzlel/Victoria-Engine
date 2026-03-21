#include "display_manager_wayland.h"
#if PLATFORM_LINUX
#	ifdef WAYLAND_ENABLED

#		include "core/error/error_macros.h"
#		include "core/os/memory.h"
#		include "core/os/os.h"
#		include "drivers/opengl/rendering_server_gl.h"
#		include "main/main.h"

#		include <string.h>

// Override from the default `wl_array_for_each` as it doesn't work for C++ compilers.
// Workaround specifically from https://github.com/libretro/RetroArch/commit/8e638f435a37c46195aad1589ab024e443971d12
// (thanks RetroArch)
#		define WL_ARRAY_FOR_EACH(pos, array, type)                                                                   \
			for (pos = (type)(array)->data; (const char *)pos < ((const char *)(array)->data + (array)->size); (pos)++)

void DisplayManagerWayland::_on_registry_global(void *p_data,
												struct wl_registry *p_registry,
												u32 p_name,
												const char *p_interface,
												u32 p_version) {
	ClientData *data = (ClientData *)p_data;
	if (strcmp(p_interface, wl_compositor_interface.name) == 0) {
		data->compositor = (struct wl_compositor *)wl_registry_bind(p_registry, p_name, &wl_compositor_interface, 4);
	}

	if (strcmp(p_interface, xdg_wm_base_interface.name) == 0) {
		data->wm_base = (struct xdg_wm_base *)wl_registry_bind(p_registry, p_name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(data->wm_base, &wm_base_listener, data);
	}

	if (strcmp(p_interface, zxdg_decoration_manager_v1_interface.name) == 0) {
		data->decor_manager = (struct zxdg_decoration_manager_v1 *)
			wl_registry_bind(p_registry, p_name, &zxdg_decoration_manager_v1_interface, 1);
	}
}

void DisplayManagerWayland::_on_registry_global_remove(void *p_data, struct wl_registry *p_registry, u32 p_name) {}

void DisplayManagerWayland::_on_xdg_surface_configure(void *p_data, struct xdg_surface *p_surface, u32 p_serial) {
	// Acknowledge, but do nothing. In most cases EGL will look after window data.
	xdg_surface_ack_configure(p_surface, p_serial);
}

void DisplayManagerWayland::_on_xdg_toplevel_configure(void *p_data,
													   struct xdg_toplevel *p_toplevel,
													   i32 width,
													   i32 height,
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

	// TODO: Update window size if not equal to zero (means the compositor wants to configure window size)
}

void DisplayManagerWayland::_on_xdg_toplevel_close(void *p_data, struct xdg_toplevel *p_toplevel) {
	OS::get_singleton()->set_exit_code(0);
	Main::set_should_quit(true);
}

void DisplayManagerWayland::_on_xdg_wm_base_ping(void *p_data, struct xdg_wm_base *p_base, u32 p_serial) {
	xdg_wm_base_pong(p_base, p_serial);
}

void DisplayManagerWayland::_on_zxdg_decoration_manager_configure(void *p_data,
																  struct zxdg_toplevel_decoration_v1 *p_decor,
																  u32 p_mode) {
	if (p_mode != ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE) {
		ERR_WARN("Victoria Engine does not support client-side decorations!");
	}
}

DisplayManager *DisplayManagerWayland::create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	return vnew(DisplayManagerWayland(p_renderer, p_size, r_error));
}

void DisplayManagerWayland::register_wayland_driver() {
	_create_funcs[create_func_count].func = create_func;
	_create_funcs[create_func_count].name = String("wayland");
	create_func_count++;
}

u8 DisplayManagerWayland::create_window(const String &p_name,
										u16 x,
										u16 y,
										u16 width,
										u16 height,
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
	wd->position = Vector2i(x, y);
	wd->notification_callback.connect(static_callable_mp(_notification_callback), false);

	if (egl_manager_wl) {
		wd->egl_window = wl_egl_window_create(wd->wl_surface, width, height);
		u8 id = egl_manager_wl->create_window(display, wd->egl_window);
		ERR_FAIL_COND_MSG_R(id == (u8)-1, "Failed to create Wayland EGL window.", INVALID_WINDOW_ID);
		wd->id = id;
	}

	return wd->id;
}

void DisplayManagerWayland::destroy_window(u8 p_id) {
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

void DisplayManagerWayland::set_window_resize_callback(const CallableMethod &p_method, u8 p_id) {
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

	if (wd->resizing || wd->maximised) {
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

		RenderingServerGL::make_default();
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

#	endif // WAYLAND_ENABLED
#endif	   // PLATFORM_LINUX
