#pragma once

#include "core/typedefs.h"
#if PLATFORM_LINUX
#	ifdef WAYLAND_ENABLED

#		include "gl_manager_egl_wayland.h"
#		include "protocols/wayland.gen.h"
#		include "protocols/xdg_decoration.gen.h"
#		include "protocols/xdg_shell.gen.h"
#		include "protocols/cursor_shape.gen.h"

#		include "core/data/event.h"
#		include "core/os/display_manager.h"

#		include <wayland-client-core.h>
#		include <wayland-egl-core.h>

class DisplayManagerWayland : public DisplayManager {
private:
	struct wl_display *display = nullptr;

	// The EGL manager for this instance.
	EGLManagerWayland *egl_manager_wl = nullptr;

	// Data passed from the compositor to the Wayland client.
	struct ClientData {
		struct wl_compositor *compositor = nullptr;
		struct zxdg_decoration_manager_v1 *decor_manager = nullptr;
		struct xdg_wm_base *wm_base = nullptr;
		struct wl_seat *seat = nullptr;
		struct wl_pointer *pointer = nullptr;
		struct wl_keyboard *keyboard = nullptr;
		struct wp_cursor_shape_manager_v1 *wp_cursor_shape = nullptr;
		struct wp_cursor_shape_device_v1 *wp_pointer_shape = nullptr;

		bool should_quit = false;
		bool frame_recieved = false;
		uint8_t active_window = INVALID_WINDOW_ID;
	};

	ClientData *client_data = nullptr;

	static void _on_registry_global(void *p_data,
									struct wl_registry *p_registry,
									uint32_t p_name,
									const char *p_interface,
									uint32_t p_version);
	static void _on_registry_global_remove(void *p_data, struct wl_registry *p_registry, uint32_t p_name);

	static void _on_seat_capabilities_changed(void *p_data, struct wl_seat *p_seat, uint32_t p_capabilities);
	static void _on_seat_name_changed(void *p_data, struct wl_seat *p_seat, const char *p_name);

	static void _on_pointer_enter(void *p_data,
								  struct wl_pointer *p_pointer,
								  uint32_t p_serial,
								  struct wl_surface *p_surface,
								  wl_fixed_t p_surface_x,
								  wl_fixed_t p_surface_y);
	static void
	_on_pointer_leave(void *p_data, struct wl_pointer *p_pointer, uint32_t p_serial, struct wl_surface *p_surface);
	static void _on_pointer_motion(void *p_data,
								   struct wl_pointer *p_pointer,
								   uint32_t p_time,
								   wl_fixed_t p_surface_x,
								   wl_fixed_t p_surface_y);
	static void _on_pointer_button(void *p_data,
								   struct wl_pointer *p_pointer,
								   uint32_t p_serial,
								   uint32_t p_time,
								   uint32_t p_button,
								   uint32_t p_state);
	static void
	_on_pointer_axis(void *p_data, struct wl_pointer *p_pointer, uint32_t p_time, uint32_t p_axis, wl_fixed_t p_fixed);
	static void _on_pointer_frame(void *p_data, struct wl_pointer *p_pointer);

	static void _on_xdg_surface_configure(void *p_data, struct xdg_surface *p_surface, uint32_t p_serial);

	static void _on_xdg_toplevel_configure(void *p_data,
										   struct xdg_toplevel *p_toplevel,
										   int32_t width,
										   int32_t height,
										   struct wl_array *p_states);
	static void _on_xdg_toplevel_close(void *p_data, struct xdg_toplevel *p_toplevel);

	static void _on_xdg_wm_base_ping(void *p_data, struct xdg_wm_base *p_base, uint32_t p_serial);

	static void
	_on_zxdg_decoration_manager_configure(void *p_data, struct zxdg_toplevel_decoration_v1 *p_decor, uint32_t p_mode);

	static constexpr struct wl_registry_listener global_listener {
		.global = DisplayManagerWayland::_on_registry_global,
		.global_remove = DisplayManagerWayland::_on_registry_global_remove,
	};

	static constexpr struct wl_seat_listener seat_listener {
		.capabilities = _on_seat_capabilities_changed, .name = _on_seat_name_changed,
	};

	static constexpr struct wl_pointer_listener pointer_listener {
		.enter = _on_pointer_enter, .leave = _on_pointer_leave, .motion = _on_pointer_motion,
		.button = _on_pointer_button, .axis = _on_pointer_axis, .frame = _on_pointer_frame,
	};

	static constexpr struct xdg_surface_listener surface_listener {
		.configure = DisplayManagerWayland::_on_xdg_surface_configure,
	};

	static constexpr struct xdg_toplevel_listener toplevel_listener {
		.configure = DisplayManagerWayland::_on_xdg_toplevel_configure,
		.close = DisplayManagerWayland::_on_xdg_toplevel_close,
	};

	static constexpr struct xdg_wm_base_listener wm_base_listener {
		.ping = DisplayManagerWayland::_on_xdg_wm_base_ping,
	};

	static constexpr struct zxdg_toplevel_decoration_v1_listener toplevel_decor_listener {
		.configure = DisplayManagerWayland::_on_zxdg_decoration_manager_configure,
	};

	struct WindowData {
		uint8_t id;
		Vector2i size;
		Vector2i cached_size;
		Vector2i position;
		Event<WindowNotification, uint8_t> notification_callback;
		CallableMethod resize_callback;

		Vector2i last_cursor_pos;
		Vector2i cursor_pos;
		bool is_first_frame;

		bool maximised = false;
		bool fullscreen = false;
		bool resizing = false;
		bool is_size_dirty = false;

		struct wl_surface *wl_surface;
		struct xdg_surface *xdg_surface;
		struct wl_egl_window *egl_window;
		struct xdg_toplevel *toplevel;
		struct zxdg_toplevel_decoration_v1 *toplevel_decoration;
	};

	WindowData *wd = nullptr;

	uint8_t _get_window_id_from_surface(struct wl_surface *p_surface);
	void _window_update_cursor_position(uint8_t p_window, const Vector2i &p_position);
	void _window_push_event(uint8_t p_window, WindowNotification p_notification);

public:
	static DisplayManager *create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error);
	static void register_wayland_driver();

	virtual uint8_t create_window(const String &p_name,
								  uint16_t x,
								  uint16_t y,
								  uint16_t width,
								  uint16_t height,
								  WindowFlags p_flags = WINDOW_FLAG_DEFAULT) override;
	virtual void destroy_window(uint8_t p_id) override;

	virtual void set_use_vsync(bool p_value) override;

	virtual Vector2i get_window_rect() const override;
	virtual void set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) override;

	virtual void toggle_mouse_mode(bool p_mode) override;
	virtual bool get_mouse_mode() const override;

	virtual void process_events() override;
	virtual void swap_buffers() override;

	virtual void finalize() override;

	DisplayManagerWayland(const String &p_renderer, const Vector2i &p_size, Error *r_error);
	~DisplayManagerWayland();
};

#	endif // WAYLAND_ENABLED
#endif	   // PLATFORM_LINUX
