#include "main.h"

#include "core/os/display_manager.h"
#include "core/os/os.h"

#include "thirdparty/glad/glad.h"

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
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    DisplayManager::get_singleton()->swap_buffers();

    return should_quit;
}

void Main::finalize() {
    DisplayManager::get_singleton()->finalize();
}
