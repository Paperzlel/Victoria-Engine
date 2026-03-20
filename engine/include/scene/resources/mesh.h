#pragma once

#include "core/typedefs.h"
#include "core/data/rid.h"
#include "core/io/resource.h"
#include "core/variant/array.h"

#include "material.h"

class VAPI Mesh : public Resource {
    VREGISTER_CLASS(Mesh, Resource)

    RID mesh;
    Ref<Material> material;

    bool mesh_is_2d = false;
    Vector3 size;

    Vector3 _mesh_get_size(const Vector3Array &p_vertices) const;
public:

    enum ArrayType {
        ARRAY_VERTEX,
        ARRAY_NORMAL,
        ARRAY_UV,
        ARRAY_INDEX,
        ARRAY_MAX
    };

    RID get_mesh() const;
    void set_mesh(RID p_mesh);

    Ref<Material> get_material() const;
    void set_material(const Ref<Material> &p_material);

    void create_from_array(const Array &p_arr);

    void force_2d();
    bool is_2d() const;

    Vector3 get_size() const;

    Mesh();
    ~Mesh();
};