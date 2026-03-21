#include "scene/3d/visual_instance_3d.h"

#include "servers/rendering_server.h"

RID VisualInstance3D::get_instance() const {
	return instance;
}

RID VisualInstance3D::get_base() const {
	return base;
}

void VisualInstance3D::set_base(RID p_base) {
	RS::get_singleton()->instance_set_base(instance, p_base);
	base = p_base;
}

void VisualInstance3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_TRANSFORM_CHANGED: {
			RS::get_singleton()->instance_set_transform(instance, get_global_transform());
		} break;
	}
}

VisualInstance3D::VisualInstance3D() {
	instance = RS::get_singleton()->instance_allocate();
}

VisualInstance3D::~VisualInstance3D() {
	RS::get_singleton()->instance_free(instance);
}