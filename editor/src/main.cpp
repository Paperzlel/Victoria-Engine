#include "editor_main.h"

#include <core/os/os.h>
#include <core/typedefs.h>
#include <libvictoria.h>
#include <scene/main/scene_tree.h>
#include <scene/main/window.h>

int main(int argc, char *argv[]) {
	VictoriaInstance *inst = victoria_create_instance(argc, argv);
	if (!inst) {
		return -1;
	}

	// Initialize editor class here
	Editor *editor = vnew(Editor);
	SceneTree::get_singleton()->get_root()->add_child(editor);

	while (inst->iteration()) {
		// Run
	}

	int exit_code = OS::get_singleton()->get_exit_code();
	victoria_destroy_instance(inst);
	return exit_code;
}
