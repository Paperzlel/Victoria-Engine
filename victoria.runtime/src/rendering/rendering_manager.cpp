#include "rendering/rendering_manager.h"

#include <core/os/display_manager.h>

RenderingManager *(*RenderingManager::_create_func)() = nullptr;

RenderingManager *RenderingManager::singleton = nullptr;

RenderingManager *RenderingManager::get_singleton() {
	return singleton;
}

RenderingManager *RenderingManager::create() {
	if (_create_func) {
		return _create_func();
	} else {
		ERR_WARN("Could not create a valid rendering backend as none were registered.");
		return nullptr;
	}
}

void RenderingManager::draw() {
	Vector2i screen_rect = DisplayManager::get_singleton()->get_window_rect();
	Mat4 screen_mat = Mat4::orthographic(0, screen_rect.y, 0, screen_rect.x, 100, -100);
	frame_data->screen_matrix = screen_mat;
	frame_data->primitive_count = 0;
	frame_data->draw_calls = 0;

	_render_internal(frame_data);
}

RenderData *RenderingManager::get_render_data() {
	return frame_data;
}

RenderingManager::RenderingManager() {
	singleton = this;

	frame_data = vnew(RenderData);
}

RenderingManager::~RenderingManager() {
	vdelete(frame_data);
}
