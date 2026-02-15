#include <core/typedefs.h>
#if PLATFORM_LINUX

#include "os_linux.h"
#include <main/main.h>

#include <libvictoria.h>

static OSLinux *os = nullptr;
static VictoriaInstance *instance = nullptr;

VictoriaInstance *victoria_create_instance(int argc, char *argv[]) {
    ERR_FAIL_COND_MSG_R(instance != nullptr, "Cannot create another Victoria instance whilst one already exists.", nullptr);
    os = new OSLinux();

    Error err = Main::setup(argc, argv);
    if (err != OK) {
        return nullptr;
    }

    // Allocate instance but don't call initialize.
    instance = vnew(VictoriaInstance);
    return instance;
}

void victoria_destroy_instance(VictoriaInstance *p_ptr) {
    if (p_ptr == instance) {
        instance->finalize();
        vdelete(instance);
        instance = nullptr;
        Main::finalize();
    }
}

#endif // PLATFORM_LINUX