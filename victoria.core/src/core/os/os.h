#pragma once

#include "core/error/error_macros.h"
#include "core/io/logger.h"
#include "core/string/vstring.h"

class MainLoop;

class VAPI OS {
protected:
	static OS *singleton;

	Logger *_logger = nullptr;

	uint8_t exit_code = 0;

	uint64_t timer_frequency = 0;
	uint64_t timer_offset = 0;

	bool stdout_verbose = false;
	bool suspended = false;
	bool should_quit = false;

	String device_vendor;
	String device_name;
	String rendering_driver;
	bool gles_over_gl;

public:
	static OS *get_singleton();
	static OS *create();
	static void destroy();

	uint8_t get_exit_code() const;
	void set_exit_code(uint8_t p_id);

	bool is_suspended() const;
	void set_is_suspended(bool p_value);

	bool is_going_to_quit() const {
		return should_quit;
	}

	void set_should_quit(bool p_value) {
		should_quit = p_value;
	}

	uint64_t get_timer_frequency() {
		return timer_frequency;
	}

	String get_rendering_driver() const;
	void set_rendering_driver(const String &p_driver);

	bool is_gles_over_gl() const {
		return gles_over_gl;
	}

	void set_gles_over_gl(bool p_value) {
		gles_over_gl = p_value;
	}

	bool is_stdout_verbose() const;
	void set_stdout_verbose(bool p_value);

	virtual void alert(const char *message, const char *title) const = 0;

	virtual void print(const char *message, ...) = 0;
	virtual void print_error(const char *file,
							 const char *function,
							 int line,
							 const char *err_msg,
							 const char *extra_msg = "",
							 ErrorType type = ERROR_ERR) = 0;
	virtual void printerr(const char *message, ...) = 0;

	virtual uint64_t get_current_time_usec() = 0;
	virtual double get_os_running_time() = 0;

	virtual bool has_feature(const String &p_feature) const = 0;
	virtual void set_feature(const String &p_feature, bool p_value) = 0;

	virtual String get_environment(const String &p_name) const = 0;
	virtual void set_environment(const String &p_name, const String &p_value) = 0;

	virtual String get_name() const = 0;
	virtual String get_version() const = 0;

	virtual String get_executable_path() const = 0;

	virtual String get_device_name() const {
		return device_name;
	}
	virtual String get_device_vendor() const {
		return device_vendor;
	}

	virtual void set_device_name(const String &p_name) {
		device_name = p_name;
	}
	virtual void set_device_vendor(const String &p_vendor) {
		device_vendor = p_vendor;
	}

	virtual int get_preferred_display_manager() = 0;

	virtual MainLoop *get_main_loop() const = 0;
	virtual void set_main_loop(MainLoop *p_tree) = 0;
	virtual void delete_main_loop() = 0;

	virtual void initialize() = 0;

	OS();
	virtual ~OS();
};
