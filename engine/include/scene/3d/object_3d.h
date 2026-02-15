#pragma once

#include <core/typedefs.h>
#include <core/math/transform_3d.h>

#include <scene/main/object.h>

class VAPI Object3D : public Object {
    VREGISTER_CLASS(Object3D, Object)

    struct Data {
        mutable Transform3D local_transform;
        mutable Transform3D global_transform;
        bool transforms_dirty;
    } data;

    void _propagate_transform_changed(Object3D *p_object);
protected:

    void _notification(int p_what);
public:

    Vector3 get_position() const;
    Quaternion get_rotation() const;
    Vector3 get_scale() const;

    void set_position(const Vector3 &p_position);
    void set_rotation(const Quaternion &p_rotation);
    void set_scale(const Vector3 &p_scale);

    Transform3D get_transform() const;
    void set_transform(const Transform3D &p_transform);
    
    Transform3D get_global_transform() const;
    void set_global_transform(const Transform3D &p_transform);

    void translate(const Vector3 &p_translation);
    void translate_global(const Vector3 &p_translation);

    void rotate(const Quaternion &p_rotation);
    void rotate_global(const Quaternion &p_rotation);

    void rotate_x(double p_angle);
    void rotate_y(double p_angle);
    void rotate_z(double p_angle);
    void rotate_xyz(double p_x, double p_y, double p_z);

    void scale(const Vector3 &p_scale);
    void scale_global(const Vector3 &p_scale);

    void look_at(const Vector3 &p_target, const Vector3 &p_up = Vector3(0, 1, 0));
    void look_at_from(const Vector3 &p_position, const Vector3 &p_target, const Vector3 &p_up = Vector3(0, 1, 0));

    Object3D();
    ~Object3D();
};
