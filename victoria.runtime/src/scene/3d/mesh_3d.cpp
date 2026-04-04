#include "scene/3d/mesh_3d.h"

Ref<Mesh> Mesh3D::get_mesh() const {
	return mesh;
}

void Mesh3D::set_mesh(const Ref<Mesh> &p_mesh) {
	if (mesh.is_valid()) {
		mesh.unref();
	}

	mesh = p_mesh;
	set_base(mesh->get_mesh());
}

Mesh3D::Mesh3D() {}

Mesh3D::~Mesh3D() {
	if (mesh.is_valid()) {
		mesh.unref();
	}
}