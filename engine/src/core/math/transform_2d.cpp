#include "core/math/transform_2d.h"

double Transform2D::get_rotation() const {
    return basis.get_rotation();
}

void Transform2D::set_rotation(double p_rotation) {
    basis.set_rotation(p_rotation);
}

Vector2 Transform2D::get_scale() const {
    return basis.get_scale();
}

void Transform2D::set_scale(const Vector2 &p_scale) {
    basis.set_scale(p_scale);
}

void Transform2D::translate(const Vector2 &p_translation) {
    *this = translated(p_translation);
}

Transform2D Transform2D::translated(const Vector2 &p_translation) {
    return Transform2D(basis, p_translation);
}

void Transform2D::rotate(double p_rotation) {
    *this = rotated(p_rotation);
}

Transform2D Transform2D::rotated(double p_rotation) {
    Mat2 b(p_rotation);
    return Transform2D(b * basis, (b * position));
}

void Transform2D::scale(const Vector2 &p_scale) {
    *this = scaled(p_scale);
}

Transform2D Transform2D::scaled(const Vector2 &p_scale) {
    Mat2 b;
    b.set_scale(p_scale);
    return Transform2D(b * basis, (b * position));
}

void Transform2D::translate_local(const Vector2 &p_translation) {
    for (int i = 0; i < 2; i++) {
        position[i] += Vector2(basis.data[i * 2], basis.data[i * 2 + 1]).dot(p_translation);
    }
}

Transform2D Transform2D::translated_local(const Vector2 &p_translation) {
    return Transform2D(basis, position + (basis * p_translation));
}

void Transform2D::rotate_local(double p_rotation) {
    *this = rotated_local(p_rotation);
}

Transform2D Transform2D::rotated_local(double p_rotation) {
    Mat2 b(p_rotation);
    return Transform2D(basis * b, position);
}

void Transform2D::scale_local(const Vector2 &p_scale) {
    *this = scaled_local(p_scale);
}

Transform2D Transform2D::scaled_local(const Vector2 &p_scale) {
    Mat2 b;
    b.scale(p_scale);
    return Transform2D(basis * b, position);
}

void Transform2D::invert() {
    basis.invert();
    position = basis * position;
}

Transform2D Transform2D::inverse() const {
    Transform2D ret = *this;
    ret.invert();
    return ret;
}

void Transform2D::operator*=(const Transform2D &p_other) {
    position += basis * p_other.position;
    basis *= p_other.basis;
}

Transform2D Transform2D::operator*(const Transform2D &p_other) {
    Transform2D ret = *this;
    ret *= p_other;
    return ret;
}