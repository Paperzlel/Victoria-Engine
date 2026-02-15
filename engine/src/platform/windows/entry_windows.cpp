#include <core/typedefs.h>
#if PLATFORM_WINDOWS

#include "os_windows.h"
#include <main/main.h>

#include <libvictoria.h>

static OSWindows *os = nullptr;
static VictoriaInstance *instance = nullptr;

VictoriaInstance *victoria_create_instance(int argc, char *argv[]) {
    ERR_FAIL_COND_MSG_R(instance != nullptr, "Cannot create another Victoria instance whilst one already exists.", nullptr);
    os = new OSWindows();

    Error err = Main::setup(argc, argv);
    if (err != OK) {
        return nullptr;
    }

    instance = vnew(VictoriaInstance);
    if (!instance->initialize()) {
        return nullptr;
    }

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

#endif // PLATFORM_WINDOWS