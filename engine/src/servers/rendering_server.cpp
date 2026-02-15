#include <servers/rendering_server.h>

#include <core/os/display_manager.h>
#include <scene/main/scene_tree.h>

RenderingServer *(*RenderingServer::_create_func)() = nullptr;

RenderingServer *RenderingServer::singleton = nullptr;


RenderingServer *RenderingServer::get_singleton() {
    return singleton;
}


RenderingServer *RenderingServer::create() {
    if (_create_func) {
        return _create_func();
    } else {
        ERR_WARN("Could not create a valid rendering backend as none were registered.");
        return nullptr;
    }
}

void RenderingServer::draw() {
    Vector2i screen_rect = DisplayManager::get_singleton()->get_window_rect();
    Mat4 screen_mat = Mat4::orthographic(0, screen_rect.y, 0, screen_rect.x, 100, -100);
    frame_data->screen_matrix = screen_mat;
    frame_data->primitive_count = 0;
    frame_data->draw_calls = 0;

    _render_internal(frame_data);
}

RenderData *RenderingServer::get_render_data() {
    return frame_data;
}

RenderingServer::RenderingServer() {
    singleton = this;

    frame_data = vnew(RenderData);
}

RenderingServer::~RenderingServer() {
    vdelete(frame_data);
}