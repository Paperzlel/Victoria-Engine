#pragma once

#include "core/typedefs.h"
#if defined(PLATFORM_LINUX) && defined(WAYLAND_ENABLED)

#	include "gl_manager_egl_wayland.h"
#	include "protocols/wayland.gen.h"
#	include "protocols/xdg_decoration.gen.h"
#	include "protocols/xdg_shell.gen.h"
#	include "protocols/cursor_shape.gen.h"
#	include "protocols/pointer_constraints.gen.h"
#	include "protocols/pointer_warp.gen.h"
#	include "protocols/relative_pointer.gen.h"

#	include "core/data/event.h"
#	include "core/os/display_manager.h"

#	include <wayland-client-core.h>
#	include <wayland-egl-core.h>

class DisplayManagerWayland : public DisplayManager {
private:
	struct wl_display *display = nullptr;
	struct wl_registry *registry = nullptr;

	// The EGL manager for this instance.
	EGLManagerWayland *egl_manager_wl = nullptr;

	// Data passed from the compositor to the Wayland client.
	struct RegistryData {
		Vector<uint32_t> registry_names;

		DisplayManagerWayland *wayland = nullptr;

		struct wl_compositor *compositor = nullptr;
		uint32_t compositor_name = 0;

		struct zxdg_decoration_manager_v1 *zxdg_decor_manager = nullptr;
		uint32_t zxdg_decor_manager_name = 0;

		struct xdg_wm_base *wm_base = nullptr;
		uint32_t wm_base_name = 0;

		struct wl_seat *seat = nullptr;
		uint32_t seat_name = 0;

		struct wp_cursor_shape_manager_v1 *wp_cursor_shape = nullptr;
		uint32_t wp_cursor_shape_name = 0;

		struct zwp_relative_pointer_manager_v1 *zwp_relative_pointer = nullptr;
		uint32_t zwp_relative_pointer_name = 0;

		struct zwp_pointer_constraints_v1 *zwp_pointer_constraints = nullptr;
		uint32_t zwp_pointer_constraints_name = 0;

		struct wp_pointer_warp_v1 *wp_pointer_warp = nullptr;
		uint32_t wp_pointer_warp_name = 0;
	};

	struct PointerData {
		Vector2i position;
		Vector2i relative_position;
		uint32_t relative_motion_time;
	};

	struct SeatData {
		// Quick access to registry global
		RegistryData *rd = nullptr;

		// Quick access to main class for certain objects
		DisplayManagerWayland *wayland = nullptr;

		struct wl_seat *seat = nullptr;
		uint32_t seat_name = 0;

		struct wl_pointer *pointer = nullptr;
		struct wl_keyboard *keyboard = nullptr;
		struct wp_cursor_shape_device_v1 *wp_cursor_shape = nullptr;
		struct zwp_relative_pointer_v1 *zwp_relative_pointer = nullptr;
		struct zwp_locked_pointer_v1 *zwp_locked_pointer = nullptr;
		struct zwp_confined_pointer_v1 *zwp_confined_pointer = nullptr;

		bool frame_recieved = false;
		uint8_t active_window = INVALID_WINDOW_ID;
		uint32_t active_serial_id = 0;

		// Write pointer data here. Allows us to accumulate info prior to a `pointer_frame` event.
		PointerData pointer_data_write;
		// Read pointer data here.
		PointerData pointer_data_read;
	};

	struct WindowData {
		uint8_t id;
		Vector2i size;
		Vector2i cached_size;
		Vector2i position;
		Event<WindowNotification, uint8_t> notification_callback;
		CallableMethod resize_callback;

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

	RegistryData *rd = nullptr;
	SeatData *sd = nullptr;
	WindowData *wd = nullptr;

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
	static void _on_pointer_axis_source(void *p_data, struct wl_pointer *p_pointer, uint32_t p_axis_source);
	static void _on_pointer_axis_stop(void *p_data, struct wl_pointer *p_pointer, uint32_t p_time, uint32_t p_axis);
	static void _on_pointer_axis_discrete(void *p_data, struct wl_pointer *p_pointer, uint32_t p_axis, int p_discrete);
	static void _on_pointer_axis_value120(void *p_data, struct wl_pointer *p_pointer, uint32_t p_axis, int p_value120);
	static void _on_pointer_axis_relative_direction(void *p_data,
													struct wl_pointer *p_pointer,
													uint32_t p_axis,
													uint32_t p_direction);

	static void _on_relative_pointer_relative_motion(void *p_data,
													 struct zwp_relative_pointer_v1 *p_relative_pointer,
													 uint32_t p_utime_high,
													 uint32_t p_utime_low,
													 wl_fixed_t p_dx,
													 wl_fixed_t p_dy,
													 wl_fixed_t p_dx_unaccel,
													 wl_fixed_t p_dy_unaccel);

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

	static void _on_zwp_locked_pointer_lock(void *p_data, struct zwp_locked_pointer_v1 *p_locked_pointer);
	static void _on_zwp_locked_pointer_unlock(void *p_data, struct zwp_locked_pointer_v1 *p_locked_pointer);

	static constexpr struct wl_registry_listener global_listener = {
		.global = DisplayManagerWayland::_on_registry_global,
		.global_remove = DisplayManagerWayland::_on_registry_global_remove,
	};

	static constexpr struct wl_seat_listener seat_listener = {
		.capabilities = _on_seat_capabilities_changed,
		.name = _on_seat_name_changed,
	};

	static constexpr struct wl_pointer_listener pointer_listener = {
		.enter = _on_pointer_enter,
		.leave = _on_pointer_leave,
		.motion = _on_pointer_motion,
		.button = _on_pointer_button,
		.axis = _on_pointer_axis,
		.frame = _on_pointer_frame,
		.axis_source = _on_pointer_axis_source,
		.axis_stop = _on_pointer_axis_stop,
		.axis_discrete = _on_pointer_axis_discrete,
		.axis_value120 = _on_pointer_axis_value120,
		.axis_relative_direction = _on_pointer_axis_relative_direction,
	};

	static constexpr struct zwp_relative_pointer_v1_listener zwp_relative_pointer_listener = {
		.relative_motion = _on_relative_pointer_relative_motion,
	};

	static constexpr struct xdg_surface_listener surface_listener = {
		.configure = DisplayManagerWayland::_on_xdg_surface_configure,
	};

	static constexpr struct xdg_toplevel_listener toplevel_listener = {
		.configure = DisplayManagerWayland::_on_xdg_toplevel_configure,
		.close = DisplayManagerWayland::_on_xdg_toplevel_close,
	};

	static constexpr struct xdg_wm_base_listener wm_base_listener = {
		.ping = DisplayManagerWayland::_on_xdg_wm_base_ping,
	};

	static constexpr struct zxdg_toplevel_decoration_v1_listener toplevel_decor_listener = {
		.configure = DisplayManagerWayland::_on_zxdg_decoration_manager_configure,
	};

	static constexpr struct zwp_locked_pointer_v1_listener zwp_locked_pointer_listener = {
		.locked = _on_zwp_locked_pointer_lock,
		.unlocked = _on_zwp_locked_pointer_unlock,
	};

	static SeatData *_seat_get_seat_data(struct wl_seat *p_seat);

	uint8_t _get_window_id_from_surface(struct wl_surface *p_surface);
	WindowData *_get_window_data_from_id(uint8_t p_window);
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

#endif // defined(PLATFORM_LINUX) && defined(WAYLAND_ENABLED)
