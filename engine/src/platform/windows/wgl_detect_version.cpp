#include "wgl_detect_version.h"
#if PLATFORM_WINDOWS

#	include "os_windows.h"

#	include <windows.h>

#	define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#	define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#	define WGL_CONTEXT_FLAGS_ARB 0x2094
#	define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

#	define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

#	define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#	define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

#	define GL_VENDOR 0x1F00
#	define GL_RENDERER 0x1F01
#	define GL_VERSION 0x1F02

typedef HGLRC (*PFNWGLCREATCONTEXTATTRIBSARB)(HDC, HGLRC, const int *);
typedef const char *(*PFNWGLGETSTRING)(unsigned int);

/**
 * @brief Method to obtain the current OpenGL version from a temporary context. Currently uses a vector, but should use
 * a Dictionary in the future if needed.
 * @return A vector containing all of the information we want - first index is the GL version, second is the card
 * manufacturer, third is driver version. If the function failed, it will return an empty dictionary, and contains no
 * data. Assume this is a failure of the program.
 */
HashTable<String, String> detect_wgl_version() {
	HashTable<String, String> ret;

	LPCWSTR class_name = L"VICTORIA_WGLDetectVersionClass";
	HINSTANCE hInstance = static_cast<OSWindows *>(OS::get_singleton())->get_hinstance();

	WNDCLASSW wc = {};
	wc.lpfnWndProc = DefWindowProcW;
	wc.lpszClassName = class_name;
	wc.hInstance = hInstance;

	if (RegisterClassW(&wc)) {
		HWND hWnd = CreateWindowExW(0,
									class_name,
									L"WGLDetectVersion",
									WS_OVERLAPPEDWINDOW,
									CW_USEDEFAULT,
									CW_USEDEFAULT,
									CW_USEDEFAULT,
									CW_USEDEFAULT,
									nullptr,
									nullptr,
									hInstance,
									nullptr);

		if (hWnd) {
			HDC hDC = GetDC(hWnd);

			if (hDC) {
				PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
											 1,
											 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
											 PFD_TYPE_RGBA,
											 32,
											 0,
											 0,
											 0,
											 0,
											 0,
											 0,
											 0,
											 0,
											 0,
											 0,
											 0,
											 0,
											 0,
											 24,
											 8,
											 0,
											 PFD_MAIN_PLANE,
											 0,
											 0,
											 0,
											 0};

				int chosen_pfd = ChoosePixelFormat(hDC, &pfd);
				if (SetPixelFormat(hDC, chosen_pfd, &pfd)) {
					HGLRC hRC = wglCreateContext(hDC);
					if (hRC) {
						if (wglMakeCurrent(hDC, hRC)) {
							PFNWGLCREATCONTEXTATTRIBSARB victoria_wglCreateContextAttribsARB = nullptr;
							victoria_wglCreateContextAttribsARB =
								(PFNWGLCREATCONTEXTATTRIBSARB)wglGetProcAddress("wglCreateContextAttribsARB");
							if (victoria_wglCreateContextAttribsARB) {
								const int attribs[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,
													   4,
													   WGL_CONTEXT_MINOR_VERSION_ARB,
													   6,
													   WGL_CONTEXT_PROFILE_MASK_ARB,
													   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
													   WGL_CONTEXT_FLAGS_ARB,
													   WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
													   0};

								HGLRC new_hRC = victoria_wglCreateContextAttribsARB(hDC, hRC, attribs);
								if (new_hRC) {
									if (wglMakeCurrent(hDC, new_hRC)) {
										// Need the glGetString function from the library, so we load here
										HMODULE module = LoadLibraryW(L"opengl32.dll");
										if (!module) {
											return ret;
										}

										PFNWGLGETSTRING victoria_wglGetString = nullptr;
										victoria_wglGetString = (PFNWGLGETSTRING)GetProcAddress(module, "glGetString");
										if (victoria_wglGetString) {
											const char *version = victoria_wglGetString(GL_VERSION);
											if (version) {
												const String &vendor = String(victoria_wglGetString(GL_VENDOR))
																		   .remove_suffix(" Corporation");
												const String &device = String(victoria_wglGetString(GL_RENDERER))
																		   .remove_suffix("/PCIe/SSE2");

												int version_major = 0;
												int version_minor = 0;
												int version_patch = 0;

												sscanf(version,
													   "%d.%d.%d",
													   &version_major,
													   &version_minor,
													   &version_patch);

												const String &version =
													vformat("%d.%d.%d", version_major, version_minor, version_patch);

												ret["version"] = version;
												ret["vendor"] = vendor;
												ret["device"] = device;
											}
										}
									}
								}
								wglMakeCurrent(nullptr, nullptr);
								wglDeleteContext(hRC);
							}
						}
					}
					wglMakeCurrent(nullptr, nullptr);
					wglDeleteContext(hRC);
				}
			}
			ReleaseDC(hWnd, hDC);
		}
		DestroyWindow(hWnd);
	}
	UnregisterClassW(class_name, hInstance);

	return ret;
}

#endif // PLATFORM_WINDOWS