#include "core/typedefs.h"
#if PLATFORM_LINUX

#include "os_linux.h"
#include "main/main.h"

int main(int argc, char **argv) {
    OSLinux os;

    Error err = Main::setup(argc, argv);
    if (err != OK) {
        if (err == ERR_HELP) {
            return 0;
        }

        return 1;
    }

    if (Main::start() == OK) {
        os.run();
    } else {
        return 1;
    }
    Main::finalize();

    return os.get_exit_code();
}

#endif // PLATFORM_LINUX