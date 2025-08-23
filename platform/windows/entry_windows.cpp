#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#include "os_windows.h"

#include "main/main.h"

int main(int argc, char **argv) {
    OSWindows os(nullptr);

    Error err = Main::setup(argc, argv);
    if (err != OK) {
        if (err == ERR_HELP) {
            return EXIT_SUCCESS;
        }

        return EXIT_FAILURE;
    }

    if (Main::start() == OK) {
        os.run();
    } else {
        return EXIT_FAILURE;
    }
    Main::finalize();
    
    return os.get_exit_code();
}

#endif // PLATFORM_WINDOWS