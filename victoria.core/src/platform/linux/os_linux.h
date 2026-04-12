#pragma once

#include "core/typedefs.h"

#if PLATFORM_LINUX

#	include "core/data/hashtable.h"
#	include "core/os/os.h"

#	include <sys/utsname.h>

/**
 * @brief Extension of the OS class for the Linux operating system.
 */
class OSLinux : public OS {
private:
	MainLoop *main_loop = nullptr;

	HashTable<String, bool> features;
	utsname system_info;

public:
	virtual void print(const char *message, ...) override;
	virtual void print_error(const char *file,
							 const char *function,
							 int line,
							 const char *err_msg,
							 const char *extra_msg,
							 ErrorType type) override;
	virtual void printerr(const char *message, ...) override;

	virtual void alert(const char *message, const char *title) const override;

	virtual uint64_t get_current_time_usec() override;
	virtual double get_os_running_time() override;

	virtual bool has_feature(const String &p_feature) const override;
	virtual void set_feature(const String &p_feature, bool p_value) override;

	virtual String get_environment(const String &p_path) const override;
	virtual void set_environment(const String &p_path, const String &p_value) override;

	virtual String get_name() const override;
	virtual String get_version() const override;
	virtual String get_executable_path() const override;
	virtual int get_preferred_display_manager() override;

	virtual MainLoop *get_main_loop() const override;
	virtual void set_main_loop(MainLoop *p_tree) override;
	virtual void delete_main_loop() override;

	virtual void initialize() override;

	OSLinux();
	~OSLinux();
};

#endif // PLATFORM_LINUX
