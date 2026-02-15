#include <scene/main/scene_tree.h>
#include <scene/3d/camera_3d.h>
#include <scene/main/window.h>

SceneTree *SceneTree::singleton = nullptr;

SceneTree *SceneTree::get_singleton() {
    return singleton;
}

void SceneTree::queue_delete(Object *p_object) {
    queued_nodes_for_deletion.push_back(p_object);
}

void SceneTree::flush_delete_queue() {
    for (Object *o : queued_nodes_for_deletion) {
        o->notification(NOTIFICATION_PREDELETE);
    }

    queued_nodes_for_deletion.clear();
}

void SceneTree::propagate_tree_notification(int p_what) {
    root->propagate_notification(p_what);
}

void SceneTree::set_active_camera(Camera3D *p_camera) {
    if (active_camera) {
        active_camera->toggle_active(false);
    }
    active_camera = p_camera;
    if (active_camera) {
        active_camera->toggle_active(true);
    }
}

double SceneTree::get_update_time() const {
    return update_time;
}

Camera3D *SceneTree::get_active_camera() {
    return active_camera;
}

void SceneTree::initialize() {
    ERR_COND_NULL(root);
    root->set_tree(this);
}

void SceneTree::update(double p_delta) {
    update_time = p_delta;
    if (active_camera) {
        active_camera->_update_camera();
    }
    propagate_tree_notification(NOTIFICATION_UPDATE);

    flush_delete_queue();
}

void SceneTree::finalize() {
    flush_delete_queue();
}

SceneTree::SceneTree() {
    singleton = this;

    root = vnew(Window);
    root->set_minimum_size(Vector2i(64, 64));
    root->set_name("root");
}

SceneTree::~SceneTree() {
    set_active_camera(nullptr);
    if (root) {
        root->set_tree(nullptr);
        vdelete(root);
    }
    if (singleton == this) {
        singleton = nullptr;
    }
}