#include "gl_manager_x11.h"
#if PLATFORM_LINUX

#include "display_manager_x11.h"
#include "core/os/os.h"

typedef GLXContext (*glXCreateContextAttribsARB)(Display *, GLXFBConfig, GLXContext, bool, const int *);
typedef void (*PFNGLXSWAPINTERVALEXT)(Display *, GLXDrawable, int);

PFNGLXSWAPINTERVALEXT victoria_glXSwapIntervalEXT = nullptr;
glXCreateContextAttribsARB victoria_glXCreateContextAttribsARB = nullptr;

/**
 * @brief Creates and allocates a GLX context to the window passed in, as well as making it the current context. 
 * @param win The window one wants to make current
 * @returns `OK` if the function went well, and `ERR_CANT_CREATE` if GLX could not create the given context.
 */
Error GLManagerX11::_create_context(GLWindow &win) {
    if (!victoria_glXCreateContextAttribsARB) {
        victoria_glXCreateContextAttribsARB = (glXCreateContextAttribsARB)glXGetProcAddressARB( (const GLubyte *) "glXCreateContextAttribsARB");
    }

    ERR_COND_NULL_R(victoria_glXCreateContextAttribsARB, ERR_CANT_CREATE);

    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 6,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };

    GLXContext context = victoria_glXCreateContextAttribsARB(display, fb_config, 0, true, context_attribs);
    XSync(display, false);
    if (!context) {
        OS::get_singleton()->printerr("Could not create a GLX context.");
        return ERR_CANT_CREATE;
    }
    win.glx_context = context;

    if (!glXMakeCurrent(display, *win.win, win.glx_context)) {
        OS::get_singleton()->printerr("Could not set the current GLX context to current.");
        return ERR_CANT_CREATE;
    }

    victoria_glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXT)glXGetProcAddressARB( (const GLubyte *) "glXSwapIntervalEXT");

    return OK;
}

/**
 * @brief Method to obtain the "visual info" of a given screen, which is used in window creation to determine if a certain type of OpenGL context can
 * be created.
 * @returns A pointer to a struct of type `XVisualInfo`.
 */
XVisualInfo *GLManagerX11::get_visual_info() {
    // Adapted from https://www.khronos.org/opengl/wiki/Tutorial:_OpenGL_3.0_Context_Creation_(GLX) 
    if (visual_info) {
        return visual_info;
    }
    
    int glx_attribs[] = {
		GLX_X_RENDERABLE    , true,
		GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
		GLX_RENDER_TYPE     , GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
		GLX_RED_SIZE        , 8,
		GLX_GREEN_SIZE      , 8,
		GLX_BLUE_SIZE       , 8,
		GLX_ALPHA_SIZE      , 8,
		GLX_DEPTH_SIZE      , 24,
		GLX_STENCIL_SIZE    , 8,
		GLX_DOUBLEBUFFER    , true,
        0
    };

    int fb_count = 0;
    GLXFBConfig *fb_configs = glXChooseFBConfig(display, screen_id, glx_attribs, &fb_count);
    if (!fb_configs) {
        OS::get_singleton()->printerr("Could not retrieve a GLX framebuffer config.");
        return nullptr;
    }

    int best_fb_config = -1, worst_fb_config = -1, best_sample_count = -1, worst_sampler_count = 999;
    for (int i = 0; i < fb_count; i++) {
        XVisualInfo *vi = glXGetVisualFromFBConfig(display, fb_configs[i]);
        if (vi) {
            int sample_buffers, samples;
            glXGetFBConfigAttrib(display, fb_configs[i], GLX_SAMPLE_BUFFERS, &sample_buffers);
            glXGetFBConfigAttrib(display, fb_configs[i], GLX_SAMPLES, &samples);

            if (best_fb_config < 0 || (sample_buffers && samples > best_sample_count)) {
                best_fb_config = i;
                best_sample_count = samples;
            }
            if (worst_fb_config < 0 || !sample_buffers || samples < worst_sampler_count) {
                worst_fb_config = i;
            }

            worst_sampler_count = samples;
        }
        XFree(vi);
    }

    GLXFBConfig best_fbc = fb_configs[best_fb_config];
    XFree(fb_configs);

    XVisualInfo *visual = glXGetVisualFromFBConfig(display, best_fbc);
    if (!visual) {
        OS::get_singleton()->printerr("X11: Could not get the correct visual info for the window.");
        XCloseDisplay(display);
        return nullptr;
    }

    if (screen_id != visual->screen) {
        OS::get_singleton()->print("screen_id (%i) does not match that of visual->screen (%i)", screen_id, visual->screen);
        XCloseDisplay(display);
        return nullptr;
    }

    fb_config = best_fbc;
    visual_info = visual;
    return visual;
}

/**
 * @brief Creates a new `GLWindow` struct and allocates the needed information to it.
 * @param win A pointer to an X11 window struct
 * @returns The ID of the new window
 */
uint8 GLManagerX11::create_window(Window *win) {
    GLWindow *n_window = new GLWindow;
    n_window->id = 0;
    n_window->win = win;

    Error err = _create_context(*n_window);
    if (err != OK) {
        return -1;
    }

    _active_window = n_window;
    return _active_window->id;
}

/**
 * @brief Makes the current context null and destroys the active window.
 * @param p_id The ID of the window to destroy
 */
void GLManagerX11::destroy_window(uint8 p_id) {
    if (!_active_window) {
        return;
    }

    if (_active_window->id == p_id) {
        glXMakeCurrent(display, 0, 0);
        glXDestroyContext(display, _active_window->glx_context);
        delete _active_window;
        _active_window = nullptr;
    }
}

/**
 * @brief Sets the window with the given ID to be the active one. Sets the context of the given window to current.
 * @param p_id The ID of the window to set as current
 */
void GLManagerX11::set_active_window(uint8 p_id) {
    if (p_id == (uint8)-1) {
        return;
    }

    GLWindow *win = _active_window;

    if (!glXMakeCurrent(display, *win->win, win->glx_context)) {
        OS::get_singleton()->printerr("Could not make the new window's GLX context current.");
        return;
    }
}

/**
 * @brief Destroys the currently active window in place of a new window.
 */
void GLManagerX11::release_active_window() {
    if (!_active_window) {
        return;
    }

    destroy_window(_active_window->id);
}

bool GLManagerX11::is_using_vsync() const {
    return using_vsync;
}

void GLManagerX11::set_use_vsync(bool p_value) {
    using_vsync = p_value;
    if (!victoria_glXSwapIntervalEXT && !p_value) {
        OS::get_singleton()->printerr("Cannot disable vsync as the extension GLX_EXT_swap_control is not present.");
    } else {
        GLXDrawable drawable = glXGetCurrentDrawable();
        if (drawable) {
            victoria_glXSwapIntervalEXT(display, drawable, p_value ? 1 : 0);
        }
    }
}

/**
 * @brief The initialization step of the OpenGL manager. Obtains the display and screen ID from the DisplayManager.
 * @returns `OK` if all went well, and `ERR_NULL` if the `DisplayManagerX11` could not be found (in which case, something has gone seriously wrong)
 */
Error GLManagerX11::initialize() {
    if (!gladLoadGL()) {
        OS::get_singleton()->print_error(__FILE__, __FUNCTION__, __LINE__, "Could not load GLAD.");
		return ERR_CANT_CREATE;
    }

    return OK;
}

/**
 * @brief The finalizing step of the OpenGL manager. Releases all resources that are no longer needed, as well as the active window.
 */
void GLManagerX11::finalize() {
    release_active_window();
    XFree(visual_info);
    visual_info = nullptr;
}

/**
 * @brief Swaps around the two different framebuffers to present the current frame. Is only available if there is double framebuffer support, which for
 * our sake we will assume that you do have access to that. 
 */
void GLManagerX11::swap_buffers() const {
    if (_active_window) {
        glXSwapBuffers(display, *_active_window->win);
    }
}

/**
 * @brief Calls to redraw the current viewport to a new size. 
 * @param width The new width of the viewport
 * @param height The new height of the viewport
 */
void GLManagerX11::resize_viewport(uint16 width, uint16 height) const {
    if (!_active_window || height == 0) {
        return;
    }
    glViewport(0, 0, width, height);
}

GLManagerX11::GLManagerX11() {
    DisplayManagerX11 *ds = static_cast<DisplayManagerX11 *>(DisplayManagerX11::get_singleton());
    ERR_COND_NULL(ds);
    display = ds->get_display();
    screen_id = ds->get_screen_id();
}

#endif // PLATFORM_LINUX