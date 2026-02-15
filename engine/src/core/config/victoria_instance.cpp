#include <core/config/victoria_instance.h>
#include <core/os/display_manager.h>
#include <core/os/os.h>

#include <main/main.h>

bool VictoriaInstance::initialize() {
    started = Main::start() == OK;
    if (started) {
        OS::get_singleton()->get_main_loop()->initialize();
    }

    return started;
}

bool VictoriaInstance::iteration() {
    DisplayManager::get_singleton()->process_events();
    return Main::iteration() == false;  // !should_quit
}

void VictoriaInstance::finalize() {
    if (started) {
        OS::get_singleton()->get_main_loop()->finalize();
    }

    started = false;
}

VictoriaInstance::VictoriaInstance() {

}

VictoriaInstance::~VictoriaInstance() {

}
