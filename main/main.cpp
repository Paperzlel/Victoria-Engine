#include "main.h"

#include "platform/linux/display_manager_x11.h"
#include "platform/linux/os_linux.h"

static bool should_quit = false;

static DisplayManager *display_manager = nullptr;

int main(void) {
    OSLinux os;

    os.initialize();

    display_manager = new DisplayManagerX11;

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