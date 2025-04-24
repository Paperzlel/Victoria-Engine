#pragma once

#include "core/typedefs.h"
#if PLATFORM_LINUX

#include "core/error/error_macros.h"
#include "thirdparty/glad/glad.h"

#include <X11/Xlib.h>
#include <GL/glx.h>

class GLManagerX11 {
private: 

    /**
     * @brief Struct for all the information held by an OpenGL window.
     */
    struct GLWindow {
        // The current ID of the window
        uint8 id;

        // A pointer to the X11 window struct
        Window *win;
        // The current GLX context that the window has
        GLXContext glx_context;
    };

    // The current display connection to X11
    Display *display = nullptr;
    // The current screen ID for the X11 screen
    int screen_id = 0;
    // The framebuffer config the application is using
    GLXFBConfig fb_config = nullptr;
    // A pointer to the current visual info struct X11 uses to determine certain factors about the window
    XVisualInfo *visual_info = nullptr;

    // The major and minor versions of GLX the user has
    // int glx_version_major, glx_version_minor = 0;
    // The major, minor and patch number of OpenGL the user has
    // int gl_version_major, gl_version_minor, gl_version_patch = 0;

    // A pointer to the currently active window being used by the application
    GLWindow *_active_window = nullptr;

    bool using_vsync = false;

    Error _create_context(GLWindow &win);
public:

    XVisualInfo *get_visual_info();

    uint8 create_window(Window *win);
    void destroy_window(uint8 p_id);

    void set_active_window(uint8 p_id);
    void release_active_window();

    bool is_using_vsync() const;
    void set_use_vsync(bool p_value);

    Error initialize();
    void finalize();
    void swap_buffers() const;

    void resize_viewport(uint16 width, uint16 height) const;

    GLManagerX11();
    ~GLManagerX11() {}
};

#endif // PLATFORM_LINUX