#include <core/os/os.h>

OS *OS::singleton = nullptr;

OS *OS::get_singleton() {
    return singleton;
}

/**
 * @brief Gets the currently set return code of the application.
 * @returns The currently set exit code.
 */
u8 OS::get_exit_code() const {
    return exit_code;
}

/**
 * @brief Method that sets the exit code of the given application. Exit codes when set will not escape the application itself, and will rely on using `Main::quit()` to actually break out of the loop.
 * @param p_id The quit message used by the user to end the application loop.
 */
void OS::set_exit_code(u8 p_id) {
    exit_code = p_id;
}

bool OS::is_suspended() const {
    return suspended;
}

void OS::set_is_suspended(bool p_value) {
    suspended = p_value;
}

bool OS::is_stdout_verbose() const {
    return stdout_verbose;
}

void OS::set_stdout_verbose(bool p_value) {
    stdout_verbose = p_value;
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