#include <core/math/transform_3d.h>

/**
 * @brief Sets the position of the transform to a given value
 * @param p_position The value to apply to the transform
 */
void Transform3D::set_position(const Vector3 &p_position) {
    position = p_position;
}

/**
 * @brief Sets the rotation of the current transform to a given quaternion.
 * @param p_quaternion The rotational quaternion to apply to the transform
 */
void Transform3D::set_rotation(const Quaternion &p_quaternion) {
    Vector3 scale = basis.get_scale();
    basis = Mat4::from_scale(scale) * Mat4(p_quaternion);
    position = (basis * Mat4::translation(position)).get_translation();
}

/**
 * @brief Sets the scale of the current transform to a given value
 * @param p_scale The scale to apply to the transform
 */
void Transform3D::set_scale(const Vector3 &p_scale) {
    Quaternion rot = basis.get_quaternion();
    basis = Mat4::from_scale(p_scale) * Mat4(rot);
    position *= p_scale;
}

/**
 * @brief Translates the object from its current position to a new position.
 * @param p_offset The vector by which one wants to translate the object by
 */
void Transform3D::translate(const Vector3 &p_offset) {
    *this = translated(p_offset);
}

/**
 * @brief Returns a translated version of the current transform.
 * @param p_offset The amount one wants to offset the current point by 
 * @returns The current transform offset by the given amount
 */
Transform3D Transform3D::translated(const Vector3 &p_offset) {
    return Transform3D(basis, position + p_offset);
}

/**
 * @brief Rotates the current transform by the given quaternion.
 * @param p_quaternion The quaternion by which one wants to rotate by
 */
void Transform3D::rotate(const Quaternion &p_quaternion) {
    *this = rotated(p_quaternion);
}

/**
 * @brief Obtains a rotated version of the current transform.
 * @param p_quaternion The quaternion by which one wants to rotate by
 * @returns A rotated version of the current transform
 */
Transform3D Transform3D::rotated(const Quaternion &p_quaternion) {
    Mat4 b(p_quaternion);
    return Transform3D(b * basis, (b * Mat4::translation(position)).get_translation());
}

/**
 * @brief Scales the current transform by a given factor.
 * @param p_scale The scale factor by which one wants to scale the current transform
 */
void Transform3D::scale(const Vector3 &p_scale) {
    *this = scaled(p_scale);
}

/**
 * @brief Obtains a scaled version of the current transform.
 * @param p_scale The scale factor by which one wants to scale the current transform
 * @returns A scaled version of the current transform
 */
Transform3D Transform3D::scaled(const Vector3 &p_scale) {
    Mat4 b = Mat4::from_scale(p_scale);
    return Transform3D(b * basis, (b * Mat4::translation(position)).get_translation());
}

/**
 * @brief Translates the current transform by a given offset in local space.
 * @param p_offset The offset by which one wants to move the current point
 */
void Transform3D::translate_local(const Vector3 &p_offset) {
    for (int i = 0; i < 3; i++) {
        position[i] += Vector3(basis.data[4 * i], basis.data[4 * i + 1], basis.data[4 * i + 2]).dot(p_offset);
    } 
}

/**
 * @brief Obtains a version of the current transform that was translated locally by a given offset.
 * @param p_offset The offset by which one wants to move the current point
 * @returns A translated version of the current transform
 */
Transform3D Transform3D::translated_local(const Vector3 &p_offset) {
    return Transform3D(basis, position + (basis * Mat4::translation(p_offset)).get_translation());
}

/**
 * @brief Rotates the current transform by a given quaternion in local space.
 * @param p_quaternion The quaternion by which to rotate the transform
 */
void Transform3D::rotate_local(const Quaternion &p_quaternion) {
    *this = rotated_local(p_quaternion);
}

/**
 * @brief Obtains a rotated version of the current transform that was rotated locally about a given quaternion
 * @param p_quaternion The quaternion by which to rotate the transform
 * @returns A rotated version of the current transform
 */
Transform3D Transform3D::rotated_local(const Quaternion &p_quaternion) {
    Mat4 b(p_quaternion);
    return Transform3D(basis * b, position);
}

/**
 * @brief Scales a transform by a given scale factor in local space.
 * @param p_scale The scale factor by which to scale the transform
 */
void Transform3D::scale_local(const Vector3 &p_scale) {
    *this = scaled_local(p_scale);
}

/**
 * @brief Obtains a version of the current transform that has been scaled by a given factor in local space.
 * @param p_scale The scale factor by which to scale the transform
 * @returns A scaled version of the current transform
 */
Transform3D Transform3D::scaled_local(const Vector3 &p_scale) {
    Mat4 b = Mat4::from_scale(p_scale);
    return Transform3D(basis * b, position);
}

/**
 * @brief Sets the current transform to be a look-at matrix, a transform intended for camera viewing purposes.
 * @param p_position The desired position of the camera
 * @param p_target The position of the target of the camera
 * @param p_up The "up" vector relative to the camera
 */
void Transform3D::look_at_from(const Vector3 &p_position, const Vector3 &p_target, const Vector3 &p_up) {
    basis = Mat4::look_at(p_position, p_target, p_up);
    position = basis.get_translation();
    basis.zero_translation();
}

/**
 * @brief Obtains a transform representing a look-at matrix from the current transform.
 * @param p_target The position of the target of the camera
 * @param p_up The "up" vector relative to the camera
 * @returns A transform representing the given look-at matrix
 */
Transform3D Transform3D::look_at(const Vector3 &p_target, const Vector3 &p_up) const {
    Transform3D t = *this;
    t.basis = Mat4::look_at(t.position, p_target, p_up);
    t.position = t.basis.get_translation();
    t.basis.zero_translation();
    return t;
}

/**
 * @brief Obtains the current transform as a `Mat4` to be used within rendering APIs and so forth.
 */
Mat4 Transform3D::get_model() const {
    Mat4 model = basis;
    model.data[3] = position.x;
    model.data[7] = position.y;
    model.data[11] = position.z;
    return model;
}

/**
 * @brief Modifies the current transform to represent a series of transforms in the opposite direction to where they would usually
 * point. 
 */
void Transform3D::invert() {
    basis.invert();
    position = (basis * Mat4::translation(position)).get_translation();
}

/**
 * @brief Returns an inverted version of the current transform.
 * @returns The current transform, inverted.
 */
Transform3D Transform3D::inverse() const {
    Transform3D ret = *this;
    ret.invert();
    return ret;
}

void Transform3D::operator*=(const Transform3D &p_other) {
    position += (basis * Mat4::translation(p_other.position)).get_translation();
    basis *= p_other.basis;
}

Transform3D Transform3D::operator*(const Transform3D &p_other) {
    Transform3D t = *this;
    t *= p_other;
    return t;
}

Transform3D::Transform3D(const Mat4 &p_look_at) {
    basis = p_look_at;
    position = p_look_at.get_translation();
}