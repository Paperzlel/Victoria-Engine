#include "gl_manager_windows.h"
#if PLATFORM_WINDOWS

#include "core/os/os.h"
#include "core/os/display_manager.h"

#include "thirdparty/glad/glad.h"

// Not included in a header, so we define manually.

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

typedef HGLRC (*PFNWGLCREATCONTEXTATTRIBSARB)(HDC, HGLRC, const int *);
typedef const char *(*PFNWGLGETEXTENSIONSSTRINGARB)(HDC);
typedef bool(APIENTRY *PFNWGLSWAPINTERVALEXT)(int);

// Critical extensions (needed by the application)

PFNWGLCREATCONTEXTATTRIBSARB victoria_wglCreateContextAttribsARB = nullptr;
PFNWGLGETEXTENSIONSSTRINGARB victoria_wglGetExtensionsStringARB = nullptr;
PFNWGLSWAPINTERVALEXT victoria_wglSwapIntervalEXT = nullptr;


Error GLManagerWindows::_create_context(GLWindow &win) {
    win.gl_context = wglCreateContext(win.hDC);
    if (win.gl_context == nullptr) {
        OS::get_singleton()->printerr("Could not attach the given OpenGL context to the given device context.");
        return ERR_CANT_CREATE;
    }

    if (!wglMakeCurrent(win.hDC, win.gl_context)) {
        OS::get_singleton()->printerr("Could not make the given OpenGL context current.");
    }

    int attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,   // Want a 4.6 context (will fallback to 3.3 eventually)
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,  // Forward compatible so that it is only using up-to-date functionality
        0
    };

    // NOTE: These two function pointers COULD be in the `find_wgl_extensions` function, however due to their necessity now, it's better to pre-load them beforehand.

    victoria_wglCreateContextAttribsARB = (PFNWGLCREATCONTEXTATTRIBSARB)wglGetProcAddress("wglCreateContextAttribsARB");
    if (victoria_wglCreateContextAttribsARB == nullptr) {
        wglDeleteContext(win.gl_context);
        win.gl_context = nullptr;
        return ERR_CANT_CREATE;
    }

    HGLRC new_gl_context = victoria_wglCreateContextAttribsARB(win.hDC, win.gl_context, attribs);
    if (new_gl_context == nullptr) {
        wglDeleteContext(win.gl_context);
        win.gl_context = nullptr;
        return ERR_CANT_CREATE;
    }

    victoria_wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARB)wglGetProcAddress("wglGetExtensionsStringARB");
    if (victoria_wglGetExtensionsStringARB == nullptr) {
        wglDeleteContext(win.gl_context);
        win.gl_context = nullptr;
        return ERR_CANT_CREATE;
    }

    victoria_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXT)wglGetProcAddress("wglSwapIntervalEXT");

    if (!wglMakeCurrent(win.hDC, nullptr)) {
        OS::get_singleton()->printerr("Could not detach the given OpenGL context.");
    }

    wglDeleteContext(win.gl_context);
    win.gl_context = new_gl_context;

    if (!wglMakeCurrent(win.hDC, win.gl_context)) {
        OS::get_singleton()->printerr("Could not attach the new OpenGL context.");
        wglDeleteContext(win.gl_context);
        win.gl_context = nullptr;
        return ERR_CANT_CREATE;
    }

    return OK;
}

int GLManagerWindows::_configure_pixel_format(HDC hDC) {
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,     // Core flags
        PFD_TYPE_RGBA,                                                  // Kind of framebuffer
        32,                                                             // Colour depth
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                                                             // No. of bits in the depth buffer
        8,                                                              // No. of bits in the stencil buffer
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    int chosen_pfd = ChoosePixelFormat(hDC, &pfd);
    ERR_FAIL_COND_MSG_R(chosen_pfd == 0, "Could not find a valid pixel format.", -1);
    SetPixelFormat(hDC, chosen_pfd, &pfd);

    return chosen_pfd;
}

uint8 GLManagerWindows::create_window(HWND hWnd, HINSTANCE hInstance) {
    HDC hDC = GetDC(hWnd);

    if (hDC == NULL) {
        return -1;
    }

    GLWindow *win_data = (GLWindow *)StaticAllocator::vallocate(sizeof(GLWindow));
    StaticAllocator::vzero(win_data, sizeof(GLWindow));
    win_data->id = 0;
    win_data->hWnd = hWnd;
    win_data->hDC = hDC;

    if (_configure_pixel_format(win_data->hDC) == -1) {
        return -1;
    }

    Error err = _create_context(*win_data);
    if (err != OK) {
        return -1;
    }
    _active_window = win_data;

    return win_data->id;
}

void GLManagerWindows::destroy_window(uint8 p_id) {
    if (!_active_window) {
        return;
    }
    if (_active_window->id == p_id) {
		StaticAllocator::vfree(_active_window, sizeof(GLWindow));
        _active_window = nullptr;
    }
}

uint8 GLManagerWindows::get_active_window() {
    if (!_active_window) {
        return -1;
    }

    return _active_window->id;
}

void GLManagerWindows::set_active_window(uint8 p_id) {
    if (p_id == (uint8)-1) {
        return;
    }

    GLWindow *win = _active_window;

    if (!wglMakeCurrent(win->hDC, win->gl_context)) {
        OS::get_singleton()->printerr("Could not make the new window's OpenGL context current.");
        return;
    }
}

void GLManagerWindows::release_active_window() {
    if (!_active_window) {
        return;
    }

    if (!wglMakeCurrent(_active_window->hDC, nullptr)) {
        OS::get_singleton()->printerr("Could not release the OpenGL context from the current window.");
    }

    destroy_window(_active_window->id);
}

bool GLManagerWindows::is_using_vsync() const {
    return using_vsync;
}

void GLManagerWindows::set_use_vsync(bool p_value) {
    using_vsync = p_value;
    if (!victoria_wglSwapIntervalEXT && !p_value) {
        OS::get_singleton()->printerr("Cannot disable vsync as the extension wglSwapIntervalEXT is not present.");
    } else {
        victoria_wglSwapIntervalEXT(p_value ? 1 : 0);
    }
}

Error GLManagerWindows::initialize() {
	if (!gladLoadGL()) {
		OS::get_singleton()->print_error(__FILE__, __FUNCTION__, __LINE__, "Could not load GLAD.");
		return ERR_CANT_CREATE;
	}

	return OK;
}

void GLManagerWindows::finalize() {
    release_active_window();
}

/**
 * @brief Swaps the framebuffers of the active window.
 */
void GLManagerWindows::swap_buffers() const {
    if (_active_window) {
        SwapBuffers(_active_window->hDC);
    }
}

void GLManagerWindows::resize_viewport(uint16 width, uint16 height) const {
    if (!_active_window || height == 0) {
        return;
    }
    glViewport(0, 0, width, height);
}

#endif // PLATFORM_WINDOWS