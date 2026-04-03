#include "os_linux.h"

#include "core/error/error_macros.h"
#include "core/os/os_internal.h"
#if PLATFORM_LINUX

#	include "logger_linux.h"

#	ifdef X11_ENABLED
#		include "x11/display_manager_x11.h"
#	endif

#	ifdef WAYLAND_ENABLED
#		include "wayland/display_manager_wayland.h"
#	endif

#	include "main/main.h"

#	include <stdarg.h>
#	include <stdlib.h>
#	include <sys/utsname.h>
#	include <unistd.h>

#	if _POSIX_C_SOURCE >= 199309L
#		include <time.h>
#	else
#		error "Require a POSIX-compliant system of a high enough version!"
#	endif

OS *os_initialize_internal() {
	return new OSLinux();
}

void os_delete_internal() {
	delete static_cast<OSLinux *>(OS::get_singleton());
}

/**
 * @brief Prints a message to the currently active console.
 * @param message The message to print
 * @param ... Variadic arguments to format into the message
 */
void OSLinux::print(const char *message, ...) {
	va_list arg_ptr;
	va_start(arg_ptr, message);

	if (_logger) {
		_logger->logv(false, message, arg_ptr);
	}

	va_end(arg_ptr);
}

/**
 * @brief Prints an error message from where it was called in the application to the currently active console.
 * @param file The file the error comes from
 * @param function The function the error comes from
 * @param line The line in the file the error comes from
 * @param err_msg The error message to output
 * @param extra_msg Any extra details about the error to give the user
 */
void OSLinux::print_error(const char *file,
						  const char *function,
						  int line,
						  const char *err_msg,
						  const char *extra_msg,
						  ErrorType type) {
	if (_logger) {
		_logger->log_error(file, function, line, err_msg, extra_msg, (Logger::ErrorType)type);
	}
}

/**
 * @brief Prints a message to the console highlighted as an error.
 * @param message The message to send to the console
 * @param ... Variadic arguments to format into the message
 */
void OSLinux::printerr(const char *message, ...) {
	va_list arg_ptr;
	va_start(arg_ptr, message);

	if (_logger) {
		_logger->logv(true, message, arg_ptr);
	}

	va_end(arg_ptr);
}

/**
 * @brief Creates an alert box to display an important message to the user. Please note that this feature is not
 * supported properly on Linux, as there is no equivalent to `MessageBox` for the X11/Wayland API without using a
 * third-party library or creating our own.
 * @param message The message to display in the box
 * @param title The title of the alert box
 */
void OSLinux::alert(const char *message, const char *title) const {
	OS::get_singleton()->printerr("%s \n%s", title, message);
}

uint64_t OSLinux::get_current_time_usec() {
	timespec t;
	clock_gettime(CLOCK_MONOTONIC_RAW, &t);
	return (uint64_t)(t.tv_nsec / 1000) + t.tv_sec * 1000000; // Return it to 1us rather than ns
}

double OSLinux::get_os_running_time() {
	return (double)(get_current_time_usec() - timer_offset) / 1000000;
}

bool OSLinux::has_feature(const String &p_feature) const {
	return features[p_feature];
}

void OSLinux::set_feature(const String &p_feature, bool p_value) {
	features[p_feature] = p_value;
}

String OSLinux::get_environment(const String &p_path) const {
	char *env = secure_getenv(p_path.get_data());
	ERR_COND_NULL_MSG_R(env, vformat("Environment variable \'%s\' not found.", p_path.get_data()), String());
	return String(env);
}

void OSLinux::set_environment(const String &p_name, const String &p_value) {
	ERR_FAIL_COND_MSG(setenv(p_name, p_value, 1) == -1,
					  vformat("Failed to set the environment variable \'%s\' to value \'%s\'.",
							  p_name.get_data(),
							  p_value.get_data()));
}

String OSLinux::get_name() const {
	return system_info.sysname;
}

String OSLinux::get_version() const {
	return system_info.release;
}

String OSLinux::get_executable_path() const {
	char buf[256];
	Memory::vcopy_memory(buf, 0, 256);
	ERR_FAIL_COND_R(readlink("/proc/self/exe", buf, 256) == -1, String());
	return buf;
}

int OSLinux::get_preferred_display_manager() {
	for (int i = 0; i < DisplayManager::get_creation_func_count(); i++) {
		String name = DisplayManager::get_creation_func_name(i);
		if (name.is_empty()) {
			continue;
		}

		String display = get_environment("XDG_SESSION_TYPE");
		if (display == name) {
			return i;
		}
	}

	// If nothing is found, use default (X11) or dummy if none were registered.
	return 0;
}

MainLoop *OSLinux::get_main_loop() const {
	return main_loop;
}

void OSLinux::set_main_loop(MainLoop *p_tree) {
	main_loop = p_tree;
}

void OSLinux::delete_main_loop() {
	if (main_loop) {
		vdelete(main_loop);
	}

	main_loop = nullptr;
}

/**
 * @brief Intialization phase for the OS class.
 */
void OSLinux::initialize() {
	uname(&system_info);

	timespec t;
	clock_getres(CLOCK_MONOTONIC, &t);
	timer_frequency = t.tv_nsec * 1000000000; // NOTE: Should always be ~1ns anyway :/
	timer_offset = get_current_time_usec();

#	ifdef X11_ENABLED
	DisplayManagerX11::register_x11_driver();
#	endif // X11_ENABLED

#	ifdef WAYLAND_ENABLED
	DisplayManagerWayland::register_wayland_driver();
#	endif
}

/**
 * @brief Contains the main loop of the application. Processes window events, then iterates through the main loop.
 */
void OSLinux::run() {
	if (!main_loop) {
		return;
	}

	main_loop->initialize();

	while (true) {
		DisplayManager::get_singleton()->process_events();
		if (Main::iteration()) {
			break;
		}
	}
}

/**
 * @brief Class constructor for the Linux operating system.
 */
OSLinux::OSLinux() {
	_logger = vnew(LinuxLogger);
}

/**
 * @brief Class destructor for the Linux operating system.
 */
OSLinux::~OSLinux() {
	vdelete(_logger);
}

#endif
