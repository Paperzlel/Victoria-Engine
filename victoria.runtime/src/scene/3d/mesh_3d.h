#pragma once

#include "scene/3d/visual_instance_3d.h"
#include "scene/resources/mesh.h"

class VAPI Mesh3D : public VisualInstance3D {
	VREGISTER_CLASS(Mesh3D, VisualInstance3D)

	Ref<Mesh> mesh;

public:
	Ref<Mesh> get_mesh() const;
	void set_mesh(const Ref<Mesh> &p_mesh);

	Mesh3D();
	~Mesh3D();
};
