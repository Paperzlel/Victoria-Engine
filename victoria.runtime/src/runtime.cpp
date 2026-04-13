#include "runtime.h"

#include "importers/resource_importer_font.h"
#include "importers/resource_importer_image.h"
#include "importers/resource_importer_obj.h"
#include "rendering/opengl/rendering_server_gl.h"
#include "rendering/rendering_server.h"
#include "scene/main/scene_tree.h"
#include "scene/register_scene_classes.h"

#include <core/io/input.h>
#include <core/io/resource_importer.h>
#include <core/os/display_manager.h>
#include <core/os/os.h>

static MainLoop *main_loop = nullptr;
static RenderingServer *rendering_server = nullptr;

static uint64_t frame_count = 0;
static bool should_quit = false;

Error runtime_initialize() {
	// Create RenderingServer
	String backend = OS::get_singleton()->get_rendering_driver();
	if (backend == "opengl") {
		RenderingServerGL::make_default(OS::get_singleton()->is_gles_over_gl());
	}
	rendering_server = RenderingServer::create();
	ERR_COND_FATAL(rendering_server == nullptr);

	// Init scene classes
	register_scene_classes();

	// Rendering server sets up GLAD, which needs a context to be created, so we do this here
	Error err = RenderingServer::get_singleton()->initialize();
	ERR_FAIL_COND_R(err != OK, err);

	// Initialize resource formats (move it elsewhere if we no longer import non-binary formats)
	Ref<ResourceFormatImporterOBJ> obj_importer;
	obj_importer.instantiate();
	ResourceImporter::get_singleton()->add_importer(obj_importer);

	Ref<ResourceFormatImporterImage> image_importer;
	image_importer.instantiate();
	ResourceImporter::get_singleton()->add_importer(image_importer);

	Ref<ResourceFormatImporterFont> font_importer;
	font_importer.instantiate();
	ResourceImporter::get_singleton()->add_importer(font_importer);

	main_loop = vnew(SceneTree);
	OS::get_singleton()->set_main_loop(main_loop);

	// Initialize if runtime is set up properly.
	OS::get_singleton()->get_main_loop()->initialize();
	return OK;
}

static double frame_time_start = 0.0;
static double delta_time = 0.0;

bool runtime_iteration() {
	// Always process events prior to checking if we're suspended, since it is only changed by the processing of said
	// events.
	DisplayManager::get_singleton()->process_events();
	// TODO: OS should notify about a quit event instead of main, since that is Godot concepts and out of date.
	should_quit = OS::get_singleton()->is_going_to_quit();
	// By default, the OS should not draw if the application is in a "suspended" state.
	if (OS::get_singleton()->is_suspended()) {
		return should_quit;
	}

	frame_count++;
	frame_time_start = OS::get_singleton()->get_os_running_time();

	// Return early from a quit request, to prevent calling on NULL items in the DisplayManager
	if (should_quit) {
		return should_quit;
	}

	SceneTree::get_singleton()->update(delta_time);

	if (Input::get_singleton()->is_key_just_pressed(KEY_F2)) {
		RenderData *rd = RS::get_singleton()->get_render_data();
		OS::get_singleton()->print("Draw calls: %d\nPrimitives: %d\nOverall time: %fms",
								   rd->draw_calls,
								   rd->primitive_count,
								   rd->render_time * 1000);
	}

	RenderingServer::get_singleton()->draw();
	DisplayManager::get_singleton()->swap_buffers();

	// Move around inputs at the end of the frame
	Input::get_singleton()->update();

	double frame_time_end = OS::get_singleton()->get_os_running_time();
	delta_time = frame_time_end - frame_time_start;

	// If `should_quit` is false, then the application will not quit out. If it is true, then the main loop will break
	// and the application will close.
	return should_quit;
}

void runtime_finalize() {
	OS::get_singleton()->get_main_loop()->finalize();
	OS::get_singleton()->delete_main_loop();

	// Get rid of all the displays used up to this point
	RenderingServer::get_singleton()->finalize();
	DisplayManager::get_singleton()->finalize();
	// Remove all singletons allocated during setup()
	vdelete(rendering_server);

	unregister_scene_classes();
}
