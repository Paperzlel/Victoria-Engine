#include "os.h"

OS *OS::singleton = nullptr;

OS *OS::get_singleton() {
    return singleton;
}

/**
 * @brief Gets the currently set return code of the application.
 * @returns The currently set exit code.
 */
uint8 OS::get_exit_code() const {
    return exit_code;
}

/**
 * @brief Method that sets the exit code of the given application. Exit codes when set will not escape the application itself, and will rely on using `MainLoop::quit()` to actually break out of the loop.
 * @param p_id The quit message used by the user to end the application loop.
 */
void OS::set_exit_code(uint8 p_id) {
    exit_code = p_id;
}

/**
 * @brief OS class constructor.
 */
OS::OS() {
    singleton = this;
}

/**
 * @brief OS class destructor.
 */
OS::~OS() {
    singleton = nullptr;
}