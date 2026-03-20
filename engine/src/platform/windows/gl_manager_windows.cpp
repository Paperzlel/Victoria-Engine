#include "gl_manager_windows.h"
#if PLATFORM_WINDOWS

#include "wgl_detect_version.h"

#include "core/os/os.h"
#include "core/os/display_manager.h"

#include <thirdparty/nvapi/nvapi_minimal.h>

#include <glad/gl.h>

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

/* NVAPI things that we don't get from the header */

#define OGL_THREAD_CONTROL_ID 0x20C1221E
#define VSYNCMODE_ID 0x00A879CF

#define OGL_THREAD_CONTROL_DISABLE 0x00000002
#define VSYNCMODE_FORCEOFF 0x08416747

typedef int(__cdecl *PFN_NvAPI_Initialize)();
typedef int(__cdecl *PFN_NvAPI_Unload)();
typedef int(__cdecl *PFN_NvAPI_GetErrorMessage)(NvAPI_Status, NvAPI_ShortString);
typedef int(__cdecl *PFN_NvAPI_DRS_CreateSession)(NVDRSSessionHandle *);
typedef int(__cdecl *PFN_NvAPI_DRS_DestroySession)(NVDRSSessionHandle);
typedef int(__cdecl *PFN_NvAPI_DRS_SaveSettings)(NVDRSSessionHandle);
typedef int(__cdecl *PFN_NvAPI_DRS_LoadSettings)(NVDRSSessionHandle);
typedef int(__cdecl *PFN_NvAPI_DRS_FindProfileByName)(NVDRSSessionHandle, NvAPI_UnicodeString, NvDRSProfileHandle *);
typedef int(__cdecl *PFN_NvAPI_DRS_CreateProfile)(NVDRSSessionHandle, NVDRS_PROFILE *, NvDRSProfileHandle *);
typedef int(__cdecl *PFN_NvAPI_DRS_GetApplicationInfo)(NVDRSSessionHandle, NvDRSProfileHandle, NvAPI_UnicodeString, NVDRS_APPLICATION *);
typedef int(__cdecl *PFN_NvAPI_DRS_CreateApplication)(NVDRSSessionHandle, NvDRSProfileHandle, NVDRS_APPLICATION *);
typedef int(__cdecl *PFN_NvAPI_DRS_SetSetting)(NVDRSSessionHandle, NvDRSProfileHandle, NVDRS_SETTING *);
PFN_NvAPI_GetErrorMessage NvAPI_GetErrorMessage_ = nullptr;

//TODO: Change when game loading becomes a thing, plus using wchar instead of char by default
const wchar_t *profile_name = L"Victoria Engine NVIDIA Profile";
const wchar_t *app_name = L"victoria.exe";


static bool nvapi_check_error(const char *msg, int status) {
    if (status != 0) {
        NvAPI_ShortString err_desc = { 0 };
        NvAPI_GetErrorMessage_((NvAPI_Status)status, err_desc);
        OS::get_singleton()->printerr("%s: %s(code: %d)", msg, err_desc, status);
        return false;
    }
    return true;
}

/**
 * @brief Creates an NVIDIA profile to disable a couple of undesired settings, namely vsync being enabled globally (which we want to have control over)
 * and disabling threaded optimisation, which causes stuttering on occasion, which we also don't want.
 */
void GLManagerWindows::_create_nvapi_profile() {
    HMODULE nvapi = nullptr;

#ifdef _WIN64
    nvapi = LoadLibraryA("nvapi64.dll");
#else
    nvapi = LoadLibraryA("nvapi.dll");
#endif // _WIN64

    // Assume that failure to load implies that the GPU is non-NVIDIA related
    if (!nvapi) {
        return;
    }
    void *(__cdecl * NvAPI_QueryInterface)(u32 interface_id) = nullptr;

    NvAPI_QueryInterface = (void *(__cdecl *)(u32))GetProcAddress(nvapi, "nvapi_QueryInterface");
    if (NvAPI_QueryInterface == nullptr) {
        return; // Should probably say something here, but whatever.
    }

    // Addresses from here: https://stackoverflow.com/questions/13291783/how-to-get-the-id-memory-address-of-dll-function 

    NvAPI_GetErrorMessage_ = (PFN_NvAPI_GetErrorMessage)NvAPI_QueryInterface(0x6C2D048C);
    PFN_NvAPI_Initialize NvAPI_Initialize = (PFN_NvAPI_Initialize)NvAPI_QueryInterface(0x0150E828);
    PFN_NvAPI_Unload NvAPI_Unload = (PFN_NvAPI_Unload)NvAPI_QueryInterface(0x0D22BDD7E);
    PFN_NvAPI_DRS_CreateSession NvAPI_DRS_CreateSession = (PFN_NvAPI_DRS_CreateSession)NvAPI_QueryInterface(0x694D52E);
    PFN_NvAPI_DRS_DestroySession NvAPI_DRS_DestroySession = (PFN_NvAPI_DRS_DestroySession)NvAPI_QueryInterface(0x0DAD9CFF8);
    PFN_NvAPI_DRS_SaveSettings NvAPI_DRS_SaveSettings = (PFN_NvAPI_DRS_SaveSettings)NvAPI_QueryInterface(0x0FCBC7E14);
    PFN_NvAPI_DRS_LoadSettings NvAPI_DRS_LoadSettings = (PFN_NvAPI_DRS_LoadSettings)NvAPI_QueryInterface(0x375DBD6B);
    PFN_NvAPI_DRS_FindProfileByName NvAPI_DRS_FindProfileByName = (PFN_NvAPI_DRS_FindProfileByName)NvAPI_QueryInterface(0x7E4A9A0B);
    PFN_NvAPI_DRS_CreateProfile NvAPI_DRS_CreateProfile = (PFN_NvAPI_DRS_CreateProfile)NvAPI_QueryInterface(0x0CC176068);
    PFN_NvAPI_DRS_GetApplicationInfo NvAPI_DRS_GetApplicationInfo = (PFN_NvAPI_DRS_GetApplicationInfo)NvAPI_QueryInterface(0x0ED1F8C69);
    PFN_NvAPI_DRS_CreateApplication NvAPI_DRS_CreateApplication = (PFN_NvAPI_DRS_CreateApplication)NvAPI_QueryInterface(0x4347A9DE);
    PFN_NvAPI_DRS_SetSetting NvAPI_DRS_SetSetting = (PFN_NvAPI_DRS_SetSetting)NvAPI_QueryInterface(0x577DD202);

    // Assume we loaded the symbols correctly
    if (!nvapi_check_error("NVAPI: Initialization failed", NvAPI_Initialize())) {
        return;
    }

    NVDRSSessionHandle s_handle;
    if (!nvapi_check_error("NVAPI: Couldn't create a session", NvAPI_DRS_CreateSession(&s_handle))) {
        NvAPI_Unload();
        return;
    }

    if (!nvapi_check_error("NVAPI: Couldn't load the global settings", NvAPI_DRS_LoadSettings(s_handle))) {
        NvAPI_DRS_DestroySession(s_handle);
        NvAPI_Unload();
        return;
    }


    NvDRSProfileHandle p_handle;
    int success = NvAPI_DRS_FindProfileByName(s_handle, (NvU16 *)(profile_name), &p_handle);
    // Need to create a profile
    if (success != 0) {

        NVDRS_PROFILE profile_info;
        profile_info.version = NVDRS_PROFILE_VER;
        profile_info.isPredefined = 0;

        int i = 0;
        while (profile_name[i]) {
            profile_info.profileName[i] = profile_name[i];
            i++;
        }
        profile_info.profileName[i] = 0;

        if (!nvapi_check_error("NVAPI: Error creating profile", NvAPI_DRS_CreateProfile(s_handle, &profile_info, &p_handle))) {
            NvAPI_DRS_DestroySession(s_handle);
            NvAPI_Unload();
            return;
        }
    }

    NVDRS_APPLICATION app;
    app.version = NVDRS_APPLICATION_VER_V4;
    app.isPredefined = 0;

    success = NvAPI_DRS_GetApplicationInfo(s_handle, p_handle, (NvU16 *)(app_name), &app);
    if (success != 0) {
        // Create application info
        int i = 0;
        while(app_name[i]) {
            app.appName[i] = app_name[i];
            i++;
        }
        app.appName[i] = 0;

        memcpy(app.launcher, L"", sizeof(wchar_t));
        memcpy(app.fileInFolder, L"", sizeof(wchar_t));

        if (!nvapi_check_error("NVAPI: Error creating application information", NvAPI_DRS_CreateApplication(s_handle, p_handle, &app))) {
            NvAPI_DRS_DestroySession(s_handle);
            NvAPI_Unload();
            return;
        }
    }

    // Disable threaded optimisation
    NVDRS_SETTING thread_setting = {};
    thread_setting.version = NVDRS_SETTING_VER;
    thread_setting.settingId = OGL_THREAD_CONTROL_ID;
    thread_setting.settingType = NVDRS_DWORD_TYPE;
    thread_setting.u32CurrentValue = OGL_THREAD_CONTROL_DISABLE; // Should really be togglable later on

    if (!nvapi_check_error("NVAPI: Couldn't save the setting correctly", NvAPI_DRS_SetSetting(s_handle, p_handle, &thread_setting))) {
        NvAPI_DRS_DestroySession(s_handle);
        NvAPI_Unload();
        return;
    }

    NVDRS_SETTING vsync_setting = {};
    vsync_setting.version = NVDRS_SETTING_VER;
    vsync_setting.settingId = VSYNCMODE_ID;
    vsync_setting.settingType = NVDRS_DWORD_TYPE;
    vsync_setting.u32CurrentValue = VSYNCMODE_FORCEOFF;

    if (!nvapi_check_error("NVAPI: Couldn't set the setting correctly", NvAPI_DRS_SetSetting(s_handle, p_handle, &vsync_setting))) {
        NvAPI_DRS_DestroySession(s_handle);
        NvAPI_Unload();
        return;
    }

    if (!nvapi_check_error("NVAPI: Couldn't save the settings correctly", NvAPI_DRS_SaveSettings(s_handle))) {
        NvAPI_DRS_DestroySession(s_handle);
        NvAPI_Unload();
        return;
    }

    NvAPI_DRS_DestroySession(s_handle);
}

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
    if (!new_gl_context) {
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
    if (victoria_wglSwapIntervalEXT == nullptr) {
        OS::get_singleton()->set_feature("vsync", false);
    } else {
        OS::get_singleton()->set_feature("vsync", true);
    }

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

u8 GLManagerWindows::create_window(HWND hWnd, HINSTANCE hInstance) {
    HDC hDC = GetDC(hWnd);

    if (hDC == NULL) {
        return -1;
    }

    GLWindow *win_data = (GLWindow *)Memory::vallocate(sizeof(GLWindow));
    Memory::vzero(win_data, sizeof(GLWindow));
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

void GLManagerWindows::destroy_window(u8 p_id) {
    if (!_active_window) {
        return;
    }
    if (_active_window->id == p_id) {
        _active_window = nullptr;
    }
}

u8 GLManagerWindows::get_active_window() {
    if (!_active_window) {
        return -1;
    }

    return _active_window->id;
}

void GLManagerWindows::set_active_window(u8 p_id) {
    if (p_id == (u8)-1) {
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

    _active_window = nullptr;
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

/**
 * @brief Method to initialize the OpenGL manager. When called, it gets an OpenGL version, and queries for the WGL extensions available at a given time. It also calls to load GLAD at the same time, in order for the function pointers used by OpenGL to work
 * @return `OK` if the manager was able to set up properly, and various error codes if not.
 */
Error GLManagerWindows::initialize() {
    //_create_nvapi_profile(); // uncomment when using RenderDoc
    HashTable<String, String> version_info = detect_wgl_version();

    sscanf(version_info["version"].get_data(), "%d.%d.%d", &gl_version_major, &gl_version_minor, &gl_version_patch);
    OS::get_singleton()->set_device_name(version_info["device"]);
    OS::get_singleton()->set_device_vendor(version_info["vendor"]);

    return OK;
}

void GLManagerWindows::finalize() {
    release_active_window();

    gl_version_major = gl_version_minor = gl_version_patch = 0;
}

/**
 * @brief Swaps the framebuffers of the active window.
 */
void GLManagerWindows::swap_buffers() const {
    if (_active_window) {
        SwapBuffers(_active_window->hDC);
    }
}

void GLManagerWindows::resize_viewport(u16 width, u16 height) const {
    
}

/**
 * @brief Class destructor. Frees all the data within the vectors.
 */
GLManagerWindows::~GLManagerWindows() {
    
}

#endif // PLATFORM_WINDOWS