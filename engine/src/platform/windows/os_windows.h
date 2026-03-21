#pragma once

#include "core/typedefs.h"
#if PLATFORM_WINDOWS

#	include "core/data/hashtable.h"
#	include "core/os/os.h"
#	include "scene/main/scene_tree.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

class OSWindows : public OS {
private:
	HINSTANCE _hInstance;
	MainLoop *main_loop = nullptr;

	HashTable<String, bool> features;

public:
	virtual void alert(const char *message, const char *title) const override;

	virtual void print(const char *message, ...) override;
	virtual void print_error(const char *file,
							 const char *function,
							 int line,
							 const char *err_msg,
							 const char *extra_msg,
							 ErrorType type) override;
	virtual void printerr(const char *message, ...) override;

	virtual u64 get_current_time_usec() override;
	virtual double get_os_running_time() override;

	virtual bool has_feature(const String &p_feature) const override;
	virtual void set_feature(const String &p_feature, bool p_value) override;

	virtual String get_name() const override;
	virtual String get_version() const override;

	virtual String get_executable_path() const override;

	virtual MainLoop *get_main_loop() const override;
	virtual void set_main_loop(MainLoop *p_tree) override;
	virtual void delete_main_loop() override;

	virtual void initialize() override;
	virtual void run() override;

	HINSTANCE get_hinstance() {
		return _hInstance;
	}
	OSWindows(HINSTANCE hInstance);
	~OSWindows(); // No current shutdown methods
};

#endif // PLATFORM_WINDOWS