#include "editor_main.h"

#include <core/os/os.h>
#include <core/typedefs.h>
#include <main/main.h>
#include <runtime/main.h>
#include <scene/main/scene_tree.h>
#include <scene/main/window.h>

int main(int argc, char *argv[]) {
	(void)OS::initialize_for_tests();
	Error err = Main::setup(argc, argv);
	if (err) {
		return -1;
	}
	err = runtime_setup();
	if (err) {
		return -2;
	}

	// Initialize editor class here
	Editor *editor = vnew(Editor);
	SceneTree::get_singleton()->get_root()->add_child(editor);

	// Doesn't need to quit
	while (!runtime_iteration()) {
		// Run
	}

	int exit_code = OS::get_singleton()->get_exit_code();
	runtime_finalize();
	Main::finalize();
	OS::destroy();
	return exit_code;
}
