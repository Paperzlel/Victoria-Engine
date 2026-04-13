#include "editor_main.h"

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

	// Initialize editor class here
	Editor *editor = vnew(Editor);
	SceneTree::get_singleton()->get_root()->add_child(editor);

	// Doesn't need to quit
	while (!runtime_iteration()) {
		// Run
	}

	int exit_code = OS::get_singleton()->get_exit_code();
	runtime_finalize();
	core_finalize();
	return exit_code;
}
