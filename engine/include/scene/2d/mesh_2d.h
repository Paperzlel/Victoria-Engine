#pragma once

#include <core/typedefs.h>
#include <scene/resources/mesh.h>

#include "object_2d.h"

class VAPI Mesh2D : public Object2D {
    VREGISTER_CLASS(Mesh2D, Object2D)

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