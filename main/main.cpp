#include "main.h"

#include "core/os/display_manager.h"
#include "core/os/os.h"

static bool should_quit = false;

void Main::set_should_quit(bool p_value) {
    should_quit = p_value;
}

Error Main::setup(int argc, char *argv[]) {
    OS::get_singleton()->initialize();

    return OK;
}

Error Main::start() {
    return OK;
}

bool Main::iteration() {
    return should_quit;
}

void Main::finalize() {
    DisplayManager::get_singleton()->finalize();
}
