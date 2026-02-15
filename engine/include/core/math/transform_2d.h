#pragma once

#include <core/typedefs.h>

#include "mat2.h"
#include "vector2.h"

struct VAPI Transform2D {
    Mat2 basis;
    Vector2 position;

    FORCE_INLINE void operator=(const Transform2D &p_other) {
        basis = p_other.basis;
        position = p_other.position;
    }

    double get_rotation() const;
    void set_rotation(double p_rotation);

    Vector2 get_scale() const;
    void set_scale(const Vector2 &p_scale);

    void translate(const Vector2 &p_translation);
    Transform2D translated(const Vector2 &p_translation);
    void rotate(double p_rotation);
    Transform2D rotated(double p_rotation);
    void scale(const Vector2 &p_scale);
    Transform2D scaled(const Vector2 &p_scale);

    void translate_local(const Vector2 &p_translation);
    Transform2D translated_local(const Vector2 &p_translation);
    void rotate_local(double p_rotation);
    Transform2D rotated_local(double p_rotation);
    void scale_local(const Vector2 &p_scale);
    Transform2D scaled_local(const Vector2 &p_scale);

    void invert();
    Transform2D inverse() const;

    void operator*=(const Transform2D &p_other);
    Transform2D operator*(const Transform2D &p_other);

    Transform2D() {}
    Transform2D(const Mat2 &p_basis, const Vector2 &p_position)
        : basis(p_basis), 
        position(p_position) {
    }

};