#include "core/os/display_manager.h"

#include "platform/headless/display_manager_headless.h"

DisplayManager *DisplayManager::singleton = nullptr;

DisplayManager::DisplayManagerCreationStruct DisplayManager::_create_funcs[MAX_CREATE_FUNCS] = {
	{&DisplayManagerHeadless::create_func, "headless"}};

int DisplayManager::create_func_count = 1;

DisplayManager *DisplayManager::get_singleton() {
	return singleton;
}

DisplayManager *
DisplayManager::create(int p_func_idx, const String &p_renderer, const Vector2i &p_size, Error *r_error) {
	DisplayManager *ret = _create_funcs[p_func_idx].func(p_renderer, p_size, r_error);
	return ret;
}

int DisplayManager::get_creation_func_count() {
	return create_func_count;
}

const char *DisplayManager::get_creation_func_name(int p_id) {
	return _create_funcs[p_id].name;
}

int DisplayManager::get_display_creation_func_id(const String &p_name) {
	for (int i = 0; i < create_func_count; i++) {
		if (p_name == _create_funcs[i].name) {
			return i;
		}
	}

	return 0;
}

DisplayManager::DisplayManager() {
	singleton = this;
}

DisplayManager::~DisplayManager() {
	singleton = nullptr;
}
