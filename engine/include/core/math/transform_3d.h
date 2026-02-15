#pragma once

#include <core/typedefs.h>

#include "quaternion.h"
#include "vector3.h"
#include "mat4.h"

/**
 * @brief Struct that represents a series of translations, rotations and scaling in order to represent the total transform of a vertex. Used extensively within any
 * 3D rendering we do, in order to achieve the above operations.
 */
struct VAPI Transform3D {
    Mat4 basis;
    Vector3 position;

    FORCE_INLINE void operator=(const Transform3D &p_other) {
        basis = p_other.basis;
        position = p_other.position;
    }

    void set_position(const Vector3 &p_position);
    void set_rotation(const Quaternion &p_quaternion);
    void set_scale(const Vector3 &p_scale);

    void translate(const Vector3 &p_offset);
    Transform3D translated(const Vector3 &p_offset);
    void rotate(const Quaternion &p_quaternion);
    Transform3D rotated(const Quaternion &p_quaternion);
    void scale(const Vector3 &p_scale);
    Transform3D scaled(const Vector3 &p_scale);

    void translate_local(const Vector3 &p_offset);
    Transform3D translated_local(const Vector3 &p_offset);
    void rotate_local(const Quaternion &p_quaternion);
    Transform3D rotated_local(const Quaternion &p_quaternion);
    void scale_local(const Vector3 &p_scale);
    Transform3D scaled_local(const Vector3 &p_scale);

    void look_at_from(const Vector3 &p_position, const Vector3 &p_target, const Vector3 &p_up = Vector3(0, 1, 0));
    Transform3D look_at(const Vector3 &p_target, const Vector3 &p_up = Vector3(0, 1, 0)) const;

    Mat4 get_model() const;
    
    void invert();
    Transform3D inverse() const;
    
    void operator*=(const Transform3D &p_other);
    Transform3D operator*(const Transform3D &p_other);

    Transform3D() {}
    Transform3D(const Transform3D &p_other) :
        basis(p_other.basis),
        position(p_other.position) {
    }
    Transform3D(const Mat4 &p_basis, const Vector3 &p_position) :
        basis(p_basis),
        position(p_position) {
    }

    Transform3D(const Mat4 &p_look_at);
};
