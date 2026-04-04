#include "scene/2d/mesh_2d.h"

Ref<Mesh> Mesh2D::get_mesh() const {
	return mesh;
}

void Mesh2D::set_mesh(const Ref<Mesh> &p_mesh) {
	if (mesh.is_valid()) {
		mesh.unref();
	}

	mesh = p_mesh;
	canvas_set_mesh(mesh);
}

bool Mesh2D::is_centred() const {
	return centred;
}

void Mesh2D::set_centred(bool p_value) {
	centred = p_value;
}

Mesh2D::Mesh2D() {}

Mesh2D::~Mesh2D() {}