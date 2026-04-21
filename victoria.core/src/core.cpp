#include "core.h"

#include "core/io/input.h"
#include "core/io/resource_importer.h"
#include "core/os/display_manager.h"
#include "core/os/os.h"
#include "core/register_core_types.h"
#include "core/string/print_string.h"
#include "core/variant/variant.h"
#include "core/version.h"

static Input *inputs = nullptr;
static ResourceImporter *resource_importer = nullptr;
static DisplayManager *display_manager = nullptr;

static String version_str;
static String rendering_backend = "";

/**
 * @brief Helper method that formats the help options properly, for the sake of neatness in the console.
 */
void print_help_option(const char *command, const char *description, bool is_header = false) {
	if (is_header) {
		OS::get_singleton()->print(command);
	} else {
		char buf[25];
		Memory::vcopy_memory(buf, command, 25);
		int i = 0;
		while (command[i]) {
			buf[i] = command[i];
			i++;
			if (i > 25) {
				ERR_WARN(vformat("Command %s was longer than 24 characters.", command));
				break;
			}
		}
		for (int j = i; j < 24; j++) {
			buf[j] = ' ';
		}
		buf[24] = 0;

		OS::get_singleton()->print("\t%s\t%s", buf, description);
	}
}

/**
 * @brief Prints the command line options available whenever the user opens the application
 */
void core_print_help() {
	OS::get_singleton()->print(version_str);
	print_help_option("Main command-line options: ", "", true);
	print_help_option("-h --help", "Prints this help message");
	print_help_option("-v --verbose",
					  "Loads the engine in verbose printing mode, putting far more information into the console.");
	print_help_option("--version", "Prints the current version of the application");

	print_help_option("Windowing options:", "", true);
	print_help_option("--width", "Set the width of the window to a given amount");
	print_help_option("--height", "Set the height of the window to a given amount");

	print_help_option("Rendering options:", "", true);
	print_help_option("--rendering-driver",
					  "Load a specific rendering driver. Can choose from: \"opengl\", \"opengl_es\"");
}

/**
 * @brief The first initialisation of the engine. Sets up all the core types, determines whether the engine is a game
 * or not, parses the command-line arguments, and so on.
 * @param argc The number of arguments specified on the command line
 * @param argv A array of strings containing each argument
 * @returns `OK` if no errors were detected, and various error messages if otherwise.
 */
Error core_initialize(int argc, char *argv[]) {
	OS::create();
	OS::get_singleton()->initialize();

	// Setup version string
	version_str = vformat("Victoria Engine v%i.%i.%i_%s_%s",
						  VICTORIA_VERSION_NUMBER_MAJOR,
						  VICTORIA_VERSION_NUMBER_MINOR,
						  VICTORIA_VERSION_NUMBER_PATCH,
						  VICTORIA_VERSION_TYPE,
						  VICTORIA_BUILD_OS);

	Vector2i window_size = {1280, 720};

	register_core_types();

	// Parse command-line arguments

	List<String> args;
	for (int i = 0; i < argc; i++) {
		args.push_back(argv[i]);
	}

	List<String>::Element *e = args.front();

	while (e) {
		String arg = e->get();
		if (arg == "--help" || arg == "-h") {
			core_print_help();
			return ERR_HELP;
		}

		if (arg == "--run-tests") {
			ERR_WARN("\'--run-tests\' is a deprecated command-line option. Run the \'victoria.tests\' command-line "
					 "tool instead.");
		}

		if (arg == "-v" || arg == "--verbose") {
			OS::get_singleton()->set_stdout_verbose(true);
			print_verbose(
				"Running engine in verbose mode, remove the -v/--verbose tags when running in order to disable it.");
		}

		if (arg == "--version") {
			OS::get_singleton()->print("%s", version_str.get_data());
			return ERR_HELP;
		}

		if (arg == "--width") {
			e = e->next();
			window_size.x = e->get().to_int();
		}

		if (arg == "--height") {
			e = e->next();
			window_size.y = e->get().to_int();
		}

		if (arg == "--rendering-driver") {
			rendering_backend = e->next()->get();
			e = e->next();
		}

		e = e->next();
	}

	// Print header
	OS::get_singleton()->print(version_str.get_data());

	// Initialize basic singletons
	inputs = vnew(Input);
	resource_importer = vnew(ResourceImporter);

	// Set backend to OpenGL native if none is found
	if (rendering_backend.is_empty()) {
		rendering_backend = "opengl";
	}

	print_verbose(vformat("Setting rendering backend to %s", rendering_backend.get_data()));

	// Get the preferred display manager based on the runtime settings
	int use_display = OS::get_singleton()->get_preferred_display_manager();
	Error err = OK;
	display_manager = DisplayManager::create(use_display, rendering_backend, window_size, &err);
	if (!display_manager || err != OK) {
		for (int i = 0; i < DisplayManager::get_creation_func_count() - 1; i++) {
			display_manager = DisplayManager::create(i, rendering_backend, window_size, &err);
			if (display_manager) {
				break;
			}
		}

		ERR_COND_NULL_MSG_R(display_manager, "Failed to create an appropriate display manager.", ERR_CANT_CREATE);
		ERR_FAIL_COND_MSG_R(err != OK, "Failed to create an appropriate display manager.", err);
	}
	return OK;
}

/**
 * @brief Method that un-initializes all of the information tied into main, and calls to destroy all of the data used
 * up to this point.
 */
void core_finalize() {
	DisplayManager::get_singleton()->finalize();

	vdelete(inputs);
	vdelete(resource_importer);
	vdelete(display_manager);

	OS::destroy();
}
