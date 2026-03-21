#include "scene/resources/mesh.h"

#include "core/variant/variant.h"
#include "servers/rendering_server.h"

/**
 * @brief Obtains the bounding box of a given mesh, which can be useful in finding the size of things later on
 */
Vector3 Mesh::_mesh_get_size(const Vector3Array &p_vertices) const {
	Vector3 min_coord;
	Vector3 max_coord;
	for (int i = 0; i < p_vertices.size(); i++) {
		Vector3 vert = p_vertices[i];
		if (min_coord.x > vert.x) {
			min_coord.x = vert.x;
		}
		if (min_coord.y > vert.y) {
			min_coord.y = vert.y;
		}
		if (min_coord.z > vert.z) {
			min_coord.z = vert.z;
		}

		if (max_coord.x < vert.x) {
			max_coord.x = vert.x;
		}
		if (max_coord.y < vert.y) {
			max_coord.y = vert.y;
		}
		if (max_coord.z < vert.z) {
			max_coord.z = vert.z;
		}
	}

	return max_coord - min_coord;
}

RID Mesh::get_mesh() const {
	return mesh;
}

void Mesh::set_mesh(RID p_mesh) {
	if (mesh.is_valid()) {
		RS::get_singleton()->mesh_free(mesh);
	}

	if (p_mesh.is_valid()) {
		mesh = p_mesh;
	}
}

void Mesh::create_from_array(const Array &p_arr) {
	RS::MeshData data;

	if (mesh_is_2d) {
		data.format |= RS::FORMAT_USE_2D_VERTICES;
	}

	for (int i = 0; i < ARRAY_MAX; i++) {
		switch (i) {
			case ARRAY_VERTEX:
			case ARRAY_NORMAL: {
				Vector3Array v = p_arr[i];

				switch (i) {
					case ARRAY_VERTEX: {
						size = _mesh_get_size(v);
						data.vertex_data = v;
						data.vertex_count = v.size();
						data.normal_offset = v.get_ptr_size() / 2; // since double precision
					} break;
					case ARRAY_NORMAL: {
						data.normal_data = v;
					} break;
				}

			} break;
			case ARRAY_UV: {
				Vector2Array v = p_arr[i];
				data.uv_data = v;
			} break;
			case ARRAY_INDEX: {
				Int32Array v = p_arr[i];
				data.index_attribs = v;
				data.index_count = v.size();
			}
		}
	}

	RS::get_singleton()->mesh_set_from_data(mesh, data);
}

Ref<Material> Mesh::get_material() const {
	return material;
}

void Mesh::set_material(const Ref<Material> &p_material) {
	if (material == p_material) {
		return;
	}

	if (material.is_valid()) {
		material.unref();
	}
	material = p_material;
	RS::get_singleton()->mesh_set_material(mesh, material.is_null() ? RID() : material->get_material());
}

void Mesh::force_2d() {
	mesh_is_2d = true;
}

bool Mesh::is_2d() const {
	return mesh_is_2d;
}

Vector3 Mesh::get_size() const {
	return size;
}

Mesh::Mesh() {
	mesh = RS::get_singleton()->mesh_allocate();
}

Mesh::~Mesh() {
	RS::get_singleton()->mesh_free(mesh);
}