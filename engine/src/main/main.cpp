#include "main/main.h"

#include "core/version.h"
#include "core/config/class_registry.h"
#include "core/io/input.h"
#include "core/io/resource_importer.h"
#include "core/os/os.h"
#include "core/os/display_manager.h"
#include "core/string/print_string.h"
#include "core/variant/variant.h"
#include "core/register_core_types.h"

#include "servers/rendering_server.h"
#include "modules/importers/resource_importer_font.h"
#include "modules/importers/resource_importer_image.h"
#include "modules/importers/resource_importer_obj.h"

#include "scene/main/scene_tree.h"
#include "scene/main/window.h"
#include "scene/register_scene_classes.h"

#ifdef TESTS_ENABLED
#include "tests/test_manager.h"
#endif // TESTS_ENABLED

static MainLoop *main_loop = nullptr;
static Input *inputs = nullptr;
static ResourceImporter *resource_importer = nullptr;
static DisplayManager *display_manager = nullptr;
static RenderingServer *rendering_server = nullptr;

static String version_str;
static u64 frame_count = 0;
static bool should_quit = false;
static String rendering_backend = "";

/**
 * @brief Helper method that formats the help options properly, for the sake of neatness in the console.
 */
void print_help_option(const char *command, const char *description, bool is_header = false) {
    if (is_header) {
        OS::get_singleton()->print(command);
    } else {
        char buf[25];
        memset(buf, 0, 25);
        int i = 0;
        while(command[i]) {
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
void Main::print_help() {
    OS::get_singleton()->print(version_str);
    print_help_option("Main command-line options: ", "", true);
    print_help_option("-h --help", "Prints this help message");
    print_help_option("--run-tests", "Runs all the pre-defined unit tests for every component");
    print_help_option("-v --verbose", "Loads the engine in verbose printing mode, putting far more information into the console.");
    print_help_option("--version", "Prints the current version of the application");

    print_help_option("Windowing options:", "", true);
    print_help_option("--width", "Set the width of the window to a given amount");
    print_help_option("--height", "Set the height of the window to a given amount");

    print_help_option("Rendering options:", "", true);
    print_help_option("--rendering-driver", "Load a specific rendering driver.Can choose from: \"opengl\", \"opengl_es\"");
}

void Main::set_should_quit(bool p_value) {
    should_quit = p_value;
}

void Main::tests_entrypoint(int argc, char *argv[], bool &p_run_tests) {
#ifdef TESTS_ENABLED
    for (int i = 0; i < argc; i++) {
        if (!strncmp(argv[i], "--run-tests", 11)) {
            p_run_tests = true;
            register_all_tests();
            run_all_tests();
        }
    }
#endif
}

/**
 * @brief The first initialisation of the engine. Sets up all the core types, determines whether the engine is a game or not, parses the command-line arguments,
 * and so on.
 * @param argc The number of arguments specified on the command line
 * @param argv A array of strings containing each argument
 * @returns `OK` if no errors were detected, and various error messages if otherwise.
 */
Error Main::setup(int argc, char *argv[]) {
    OS::get_singleton()->initialize();

    // Setup version string
    version_str = vformat("Victoria Engine v%i.%i.%i_%s_%s",
        VICTORIA_VERSION_NUMBER_MAJOR,
        VICTORIA_VERSION_NUMBER_MINOR,
        VICTORIA_VERSION_NUMBER_PATCH,
        VICTORIA_VERSION_TYPE,
        VICTORIA_BUILD_OS);

    Vector2i window_size = {1280, 720 };

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
            print_help();
            return ERR_HELP;
        }

        if (arg == "--run-tests") {
            ERR_WARN("--run-tests was specified on the command line, but Victoria Engine was not compiled for unit tests. Please compile a version of the engine that has support for unit tests before using this command.");
        }

        if (arg == "-v" || arg == "--verbose") {
            OS::get_singleton()->set_stdout_verbose(true);
            print_verbose("Running editor in verbose mode, remove the -v/--verbose tags when running in order to disable it.");
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
    }

    rendering_server = RenderingServer::create();
    OS::get_singleton()->set_feature("renderer_use_opengl", true);
    ERR_COND_FATAL(rendering_server == nullptr);

    return OK;
}

/**
 * @brief Method that begins the first few moments of running the engine. When a game is exported, a
 * variable will be held somewhere in the code that tells the engine it should load like a normal game,
 * rather than like the engine itself.
 * @returns `OK` if startup finished successfully without any issues, and various error messages if not.
 */
Error Main::start() {
    register_scene_classes();

    DisplayManager::get_singleton()->set_use_vsync(false);

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

    return OK;
}

static double frame_time_start = 0.0;
static double delta_time = 0.0;

/**
 * @brief Main iteration of the application. All methods and updates are called to be queried from here.
 * @returns `0` if the operation was successful, and `>1` if it needs to quit.
 */
bool Main::iteration() {
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
        OS::get_singleton()->print("Draw calls: %d\nPrimitives: %d\nOverall time: %fms", rd->draw_calls,
                rd->primitive_count, rd->render_time * 1000);
    }

    RenderingServer::get_singleton()->draw();
    DisplayManager::get_singleton()->swap_buffers();

    // Move around inputs at the end of the frame
    Input::get_singleton()->update();

    double frame_time_end = OS::get_singleton()->get_os_running_time();
    delta_time = frame_time_end - frame_time_start;

    // If `should_quit` is false, then the application will not quit out. If it is true, then the main loop will break and the application will close.
    return should_quit;
}

/**
 * @brief Method that un-initializes all of the information tied into main, and calls to destroy all of the data used up to this point.
 */
void Main::finalize() {
    OS::get_singleton()->delete_main_loop();

    // Get rid of all the displays used up to this point
    RenderingServer::get_singleton()->finalize();
    DisplayManager::get_singleton()->finalize();
    // Remove all singletons allocated during setup()
    vdelete(rendering_server);
    vdelete(inputs);
    vdelete(resource_importer);

    unregister_scene_classes();
}
