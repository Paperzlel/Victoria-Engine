#pragma once

#include <core/typedefs.h>

#if PLATFORM_LINUX
#ifdef WAYLAND_ENABLED

#include <core/os/display_manager.h>
#include <core/data/event.h>

#include "gl_manager_egl_wayland.h"

#include <wayland-client-core.h>
#include <wayland-egl-core.h>

#include "protocols/wayland.gen.h"
#include "protocols/xdg_shell.gen.h"
#include "protocols/xdg_decoration.gen.h"

class DisplayManagerWayland : public DisplayManager {
private:

    struct wl_display *display = nullptr;

    // The EGL manager for this instance.
    EGLManagerWayland *egl_manager_wl = nullptr;

    // Data passed from the compositor to the Wayland client.
    struct ClientData {
        struct wl_compositor *compositor;
        struct zxdg_decoration_manager_v1 *decor_manager;
        struct xdg_wm_base *wm_base;

        bool should_quit = false;
    };

    ClientData *client_data = nullptr;

    static void _on_registry_global(void *p_data, struct wl_registry *p_registry, u32 p_name, const char *p_interface, u32 p_version);
    static void _on_registry_global_remove(void *p_data, struct wl_registry *p_registry, u32 p_name);

    static void _on_xdg_surface_configure(void *p_data, struct xdg_surface *p_surface, u32 p_serial);

    static void _on_xdg_toplevel_configure(void *p_data, struct xdg_toplevel *p_toplevel, i32 width, i32 height, struct wl_array *p_states);
    static void _on_xdg_toplevel_close(void *p_data, struct xdg_toplevel *p_toplevel);

    static void _on_xdg_wm_base_ping(void *p_data, struct xdg_wm_base *p_base, u32 p_serial);

    static void _on_zxdg_decoration_manager_configure(void *p_data, struct zxdg_toplevel_decoration_v1 *p_decor, u32 p_mode);

    static constexpr struct wl_registry_listener global_listener {
        .global = DisplayManagerWayland::_on_registry_global,
        .global_remove = DisplayManagerWayland::_on_registry_global_remove
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
        .configure = DisplayManagerWayland::_on_zxdg_decoration_manager_configure
    };

    struct WindowData {
        u8 id;
        Vector2i size;
        Vector2i cached_size;
        Vector2i position;
        Event<WindowNotification, u8> notification_callback;
        CallableMethod resize_callback;

        bool maximised = false;
        bool fullscreen = false;
        bool resizing = false;

        struct wl_surface *wl_surface;
        struct xdg_surface *xdg_surface;
        struct wl_egl_window *egl_window;
        struct xdg_toplevel *toplevel;
        struct zxdg_toplevel_decoration_v1 *toplevel_decoration;
    };

    WindowData *wd = nullptr;

public:

    static DisplayManager *create_func(const String &p_renderer, const Vector2i &p_size, Error *r_error);
    static void register_wayland_driver();

    virtual u8 create_window(const String &p_name, u16 x, u16 y, u16 width, u16 height, WindowFlags p_flags = WINDOW_FLAG_DEFAULT) override;
    virtual void destroy_window(u8 p_id) override;

    virtual void set_use_vsync(bool p_value) override;

    virtual Vector2i get_window_rect() const override;
    virtual void set_window_resize_callback(const CallableMethod &p_method, u8 p_id) override;

    virtual void toggle_mouse_mode(bool p_mode) override;
    virtual bool get_mouse_mode() const override;

    virtual void process_events() override;
    virtual void swap_buffers() override;

    virtual void finalize() override;

    DisplayManagerWayland(const String &p_renderer, const Vector2i &p_size, Error *r_error);
    ~DisplayManagerWayland();
};


#endif // WAYLAND_ENABLED
#endif // PLATFORM_LINUX
