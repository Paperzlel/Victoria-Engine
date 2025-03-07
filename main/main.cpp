#include "main.h"

#include "platform/windows/display_manager_windows.h"
#include "platform/windows/os_windows.h"

static bool should_quit = false;

static DisplayManager *display_manager = nullptr;

int main(void) {
    OSWindows os(nullptr);

    os.initialize();

    display_manager = new DisplayManagerWindows;

    display_manager->create_window("Victoria Engine Window", 100, 100, 1280, 720);

    os.run();

    display_manager->finalize();

    return 0;
}

bool iteration() {
    return should_quit;
}

void set_should_quit(bool p_value) {
    should_quit = p_value;
}