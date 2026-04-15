#pragma once

#include "scene/2d/game_object_2d.h"
#include "scene/resources/mesh.h"

class VAPI Mesh2D : public GameObject2D {
	VREGISTER_CLASS(Mesh2D, GameObject2D);

	Ref<Mesh> mesh;
	bool centred = false;

public:
	Ref<Mesh> get_mesh() const;
	void set_mesh(const Ref<Mesh> &p_mesh);

	bool is_centred() const;
	void set_centred(bool p_value);

	Mesh2D();
	~Mesh2D();
};