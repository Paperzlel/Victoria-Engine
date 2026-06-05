#include "editor_main.h"
#include "register_editor_classes.h"

#include <core/os/os.h>

#include <scene/main/scene_tree.h>
#include <scene/main/window.h>

#include <core.h>
#include <runtime.h>

int main(int argc, char *argv[]) {
	Error err = core_initialize(argc, argv);
	if (err) {
		return -1;
	}
	err = runtime_initialize();
	if (err) {
		return -2;
	}

	register_editor_classes();

	// Initialize editor class here
	Editor *editor = vnew(Editor);
	SceneTree::get_singleton()->get_root()->add_child(editor);

	// Doesn't need to quit
	while (!runtime_iteration()) {
		// Run
	}

	unregister_editor_classes();

	int exit_code = OS::get_singleton()->get_exit_code();
	runtime_finalize();
	core_finalize();
	return exit_code;
}

// Override required for crash handlers on Windows, this should be put into a macro for end-users
#if defined(PLATFORM_WINDOWS) && defined(_MSC_VER)

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

extern VCORE_API DWORD CrashHandlerException(EXCEPTION_POINTERS *ep);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	__try {
		Vector<char *> cmdline;
		{
			char buf[MAX_PATH] = {};
			DWORD len = GetModuleFileNameA(GetModuleHandleA(nullptr), buf, sizeof(buf));
			cmdline.push_back(buf);

			Vector<String> s = String(lpCmdLine).split(" ");
			for (int i = 0; i < s.size(); i++) {
				cmdline.push_back(s[i].ptrw());
			}
		}

		return main(cmdline.size(), cmdline.ptrw());
	} __except (CrashHandlerException(GetExceptionInformation())) {
		return 1;
	}
}

#endif // defined(PLATFORM_WINDOWS) && defined(_MSC_VER)
