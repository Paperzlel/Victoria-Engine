#pragma once

#include "core/typedefs.h"
#include "core/os/memory.h"

#include "math_funcs.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"

// Euler rotations derived from here: https://danceswithcode.net/engineeringnotes/rotations_in_3d/rotations_in_3d_part1.html 

/**
 * @brief Structure that represents a 4x4 matrix, commonly used for transforms in 3D space. Our implementation uses a row-major order, as opposed to OpenGL's
 * column-major order, and thus any matrices used within OpenGL need to be transposed prior to being taken into a vertex/fragment shader.
 */
struct Mat4 {
public:

    double data[16] = { 0 };

    /* OPERATORS */

    FORCE_INLINE double operator[](const int p_index) {
        return data[CLAMP(p_index, 0, 15)];
    }

    FORCE_INLINE const double operator[](const int p_index) const {
        return data[CLAMP(p_index, 0, 15)];
    }

    FORCE_INLINE void operator=(const Mat4 &p_other) {
        Memory::vcopy_memory(data, (void *)p_other.data, sizeof(double) * 16);
    }

    FORCE_INLINE Mat4 operator*(const Mat4 &p_other);
    FORCE_INLINE void operator*=(const Mat4 &p_other);
    FORCE_INLINE Vector4 operator*(const Vector4 &p_other);
    FORCE_INLINE Mat4 operator+(const Mat4 &p_other);
    FORCE_INLINE void operator+=(const Mat4 &p_other);

    /* STATIC TRANSFORM CONSTRUCTORS */

    static FORCE_INLINE Mat4 identity();
    static FORCE_INLINE Mat4 translation(const Vector3 &p_vec);
    static FORCE_INLINE Mat4 from_scale(const Vector3 &p_scale);

    static FORCE_INLINE Mat4 rotate_x(double angle);
    static FORCE_INLINE Mat4 rotate_y(double angle);
    static FORCE_INLINE Mat4 rotate_z(double angle);
    static FORCE_INLINE Mat4 rotate_xyz(double p_x, double p_y, double p_z);

    static FORCE_INLINE Mat4 orthographic(double top, double bottom, double left, double right, double p_near, double p_far);
    static FORCE_INLINE Mat4 perspective(double fov, double aspect_ratio, double p_near, double p_far);

    static FORCE_INLINE Mat4 look_at(Vector3 p_position, Vector3 p_target, Vector3 p_up);
    static FORCE_INLINE Mat4 model_matrix(Vector3 p_position, Quaternion p_rotation, Vector3 p_scale);

    /* MATRIX OPERATIONS */

    FORCE_INLINE double determinant() const;
    FORCE_INLINE void transpose();
    FORCE_INLINE Mat4 transposed() const;
    FORCE_INLINE void orthonormalize();
    FORCE_INLINE Mat4 orthonormalized() const;
    FORCE_INLINE void invert();
    FORCE_INLINE Mat4 inverse() const;
    FORCE_INLINE void zero_translation();

    FORCE_INLINE Vector3 get_row(int p_row);
    FORCE_INLINE Vector3 get_column(int p_column);

    FORCE_INLINE Vector3 get_euler_angles() const;
    FORCE_INLINE Quaternion get_quaternion() const;
    FORCE_INLINE Quaternion get_quaternion_unsafe() const;
    FORCE_INLINE Vector3 get_scale() const;
    FORCE_INLINE Vector3 get_translation() const;

    FORCE_INLINE void scale(const Vector3 &p_scale);
    FORCE_INLINE Mat4 scaled(const Vector3 &p_scale);
    FORCE_INLINE void rotate(const Vector3 &p_axis, double p_angle);
    FORCE_INLINE Mat4 rotated(const Vector3 &p_axis, double p_angle);
    FORCE_INLINE void rotate(const Quaternion &p_quaternion);
    FORCE_INLINE Mat4 rotated(const Quaternion &p_quaternion);

    FORCE_INLINE void scale_local(const Vector3 &p_scale);
    FORCE_INLINE Mat4 scaled_local(const Vector3 &p_scale);
    FORCE_INLINE void rotate_local(const Vector3 &p_axis, double p_angle);
    FORCE_INLINE Mat4 rotated_local(const Vector3 &p_axis, double p_angle);
    FORCE_INLINE void rotate_local(const Quaternion &p_quaternion);
    FORCE_INLINE Mat4 rotated_local(const Quaternion &p_quaternion);

    /**
     * @brief Mat4 class constructor.
     */
    Mat4() {
        data[0] = 1;
        data[5] = 1;
        data[10] = 1;
        data[15] = 1;
    }
    
    /**
     * @brief Mat4 class constructor. Assigns from another matrix.
     * @param p_from The other matrix one wants to assign from
     */
    Mat4(const Mat4 &p_from) {
        *this = p_from;
    }

    Mat4(double a1, double a2, double a3, double a4, double b1, double b2, double b3, double b4, double c1, double c2, double c3, double c4,
            double d1, double d2, double d3, double d4) {
                data[0] = a1;
                data[1] = a2;
                data[2] = a3;
                data[3] = a4;
                data[4] = b1;
                data[5] = b2;
                data[6] = b3;
                data[7] = b4;
                data[8] = c1;
                data[9] = c2;
                data[10] = c3;
                data[11] = c4;
                data[12] = d1;
                data[13] = d2;
                data[14] = d3;
                data[15] = d4;
            }

    Mat4(const Quaternion &p_quaternion) {
        *this = p_quaternion.to_rotation_matrix();
    }
};

/**
 * @brief Multiplies two matrices together. Due to the nature of matrix transformations, the order one wants to multiply two matrices by is right-to-left, such as
 * XYZ rotations being actually computed in the order ZYX.
 * @param p_other The other matrix one wants to multiply against
 * @returns The product of the two matrices
 */
Mat4 Mat4::operator*(const Mat4 &p_other) {
    Mat4 ret = *this;
    ret *= p_other;
    return ret;
}

/**
 * @brief Multiplies two matrices together.
 */
void Mat4::operator*=(const Mat4 &p_other) {
    Mat4 tmp = *this;
    data[0] = tmp.data[0] * p_other[0] + tmp.data[1] * p_other[4] + tmp.data[2] * p_other[8] + tmp.data[3] * p_other[12];
    data[1] = tmp.data[0] * p_other[1] + tmp.data[1] * p_other[5] + tmp.data[2] * p_other[9] + tmp.data[3] * p_other[13];
    data[2] = tmp.data[0] * p_other[2] + tmp.data[1] * p_other[6] + tmp.data[2] * p_other[10] + tmp.data[3] * p_other[14];
    data[3] = tmp.data[0] * p_other[3] + tmp.data[1] * p_other[7] + tmp.data[2] * p_other[11] + tmp.data[3] * p_other[15];
    data[4] = tmp.data[4] * p_other[0] + tmp.data[5] * p_other[4] + tmp.data[6] * p_other[8] + tmp.data[7] * p_other[12];
    data[5] = tmp.data[4] * p_other[1] + tmp.data[5] * p_other[5] + tmp.data[6] * p_other[9] + tmp.data[7] * p_other[13];
    data[6] = tmp.data[4] * p_other[2] + tmp.data[5] * p_other[6] + tmp.data[6] * p_other[10] + tmp.data[7] * p_other[14];
    data[7] = tmp.data[4] * p_other[3] + tmp.data[5] * p_other[7] + tmp.data[6] * p_other[11] + tmp.data[7] * p_other[15];
    data[8] = tmp.data[8] * p_other[0] + tmp.data[9] * p_other[4] + tmp.data[10] * p_other[8] + tmp.data[11] * p_other[12];
    data[9] = tmp.data[8] * p_other[1] + tmp.data[9] * p_other[5] + tmp.data[10] * p_other[9] + tmp.data[11] * p_other[13];
    data[10] = tmp.data[8] * p_other[2] + tmp.data[9] * p_other[6] + tmp.data[10] * p_other[10] + tmp.data[11] * p_other[14];
    data[11] = tmp.data[8] * p_other[3] + tmp.data[9] * p_other[7] + tmp.data[10] * p_other[11] + tmp.data[11] * p_other[15];
    data[12] = tmp.data[12] * p_other[0] + tmp.data[13] * p_other[4] + tmp.data[14] * p_other[8] + tmp.data[15] * p_other[12];
    data[13] = tmp.data[12] * p_other[1] + tmp.data[13] * p_other[5] + tmp.data[14] * p_other[9] + tmp.data[15] * p_other[13];
    data[14] = tmp.data[12] * p_other[2] + tmp.data[13] * p_other[6] + tmp.data[14] * p_other[10] + tmp.data[15] * p_other[14];
    data[15] = tmp.data[12] * p_other[3] + tmp.data[13] * p_other[7] + tmp.data[14] * p_other[11] + tmp.data[15] * p_other[15];
}

/**
 * @brief Multiplies a matrix by a coordinate vector.
 */
Vector4 Mat4::operator*(const Vector4 &p_other) {
    Vector4 ret;
    ret.x = data[0] * p_other[0] + data[1] * p_other[1] + data[2] * p_other[2] + data[3] * p_other[3];
    ret.y = data[4] * p_other[0] + data[5] * p_other[1] + data[6] * p_other[2] + data[7] * p_other[3];
    ret.z = data[8] * p_other[0] + data[9] * p_other[1] + data[10] * p_other[2] + data[11] * p_other[3];
    ret.w = data[12] * p_other[0] + data[13] * p_other[1] + data[14] * p_other[2] + data[15] * p_other[3];
    return ret;
}

/**
 * @brief Adds two matrices togther.
 * @param p_other The other matrix to add into the current matrix
 * @returns The sum of the two matrices
 */
Mat4 Mat4::operator+(const Mat4 &p_other) {
    Mat4 ret = *this;
    ret += p_other;
    return ret;
}

/**
 * @brief Adds two matrices together.
 */
void Mat4::operator+=(const Mat4 &p_other) {
    data[0] += p_other[0];
    data[1] += p_other[1];
    data[2] += p_other[2];
    data[3] += p_other[3];
    data[4] += p_other[4];
    data[5] += p_other[5];
    data[6] += p_other[6];
    data[7] += p_other[7];
    data[8] += p_other[8];
    data[9] += p_other[9];
    data[10] += p_other[10];
    data[11] += p_other[11];
    data[12] += p_other[12];
    data[13] += p_other[13];
    data[14] += p_other[14];
    data[15] += p_other[15];
}

/**
 * @brief Returns an instance of the identity matrix, of which is a matrix that when multiplied against will produce no change to the other matrix.
 */
Mat4 Mat4::identity() {
    Mat4 ret;
    ret.data[0] = 1.0;
    ret.data[5] = 1.0;
    ret.data[10] = 1.0;
    ret.data[15] = 1.0;
    return ret;
}

/**
 * @brief Creates a translation matrix in the direction given by the input vector.
 * @param p_vec The vector one wants to translate by
 */
Mat4 Mat4::translation(const Vector3 &p_vec) {
    Mat4 ret = identity();
    ret.data[3] = p_vec.x;
    ret.data[7] = p_vec.y;
    ret.data[11] = p_vec.z;
    return ret;
}

/**
 * @brief Creates a matrix which will scale an object by the given vector about the origin.
 * @param p_scale The vector one wants to scale by
 */
Mat4 Mat4::from_scale(const Vector3 &p_scale) {
    Mat4 ret = identity();
    ret.data[0] = p_scale.x;
    ret.data[5] = p_scale.y;
    ret.data[10] = p_scale.z;
    return ret;
}

/**
 * @brief Creates a matrix that rotates a given point about the origin with respect to the x-axis (pitch).
 * @param angle The angle in radians one wants to rotate by
 */
Mat4 Mat4::rotate_x(double angle) {
    Mat4 ret = identity();
    double sin = Math::sin(angle);
    double cos = Math::cos(angle);

    ret.data[5] = cos;
    ret.data[6] = -sin;
    ret.data[9] = sin;
    ret.data[10] = cos;
    return ret;
}

/**
 * @brief Creates a matrix that rotates a given point about the origin with respect to the y-axis (yaw).
 * @param angle The angle in radians one wants to rotate by
 */
Mat4 Mat4::rotate_y(double angle) {
    Mat4 ret = identity();
    double sin = Math::sin(angle);
    double cos = Math::cos(angle);

    ret.data[0] = cos;
    ret.data[2] = sin;
    ret.data[8] = -sin;
    ret.data[10] = cos;
    return ret;
}

/**
 * @brief Creates a matrix that rotates a given point about the origin with respect to the z-axis (roll).
 * @param angle The angle in radians one wants to rotate by
 */
Mat4 Mat4::rotate_z(double angle) {
    Mat4 ret = identity();

    double sin = Math::sin(angle);
    double cos = Math::cos(angle);

    ret.data[0] = cos;
    ret.data[1] = -sin;
    ret.data[4] = sin;
    ret.data[5] = cos;
    return ret;
}

/**
 * @brief Constructs a rotation matrix from three Euler angles. Order of rotation is YXZ or yaw-pitch-roll, as is the default for the program.
 */
Mat4 Mat4::rotate_xyz(double p_x, double p_y, double p_z) {
    Mat4 xm = rotate_x(p_x);
    Mat4 ym = rotate_y(p_y);
    Mat4 zm = rotate_z(p_z);
    Mat4 ret = ym * xm * zm;
    return ret;
}

/**
 * @brief Creates an orthographic project matrix, a projection by which two vertices will appear to be right next to one another despite significant differences
 * in position to the camera. The current implementation measure the height and width from the top to the bottom of the screen for the y axis, and from the left
 * to the right for the x axis.
 * @param top The highest value on the y axis the frustum has in screen coordinates
 * @param bottom The lowest value on the y axis the frustum has in screen coordinates
 * @param left The highest value on the x axis the frustum has in screen coordinates
 * @param right The lowest value on the x axis the frustum has in screen coordinates
 * @param p_near How close a vertex can get to the camera before being culled
 * @param p_far How far away a vertex can get from the camera before being culled
 */
Mat4 Mat4::orthographic(double top, double bottom, double left, double right, double p_near, double p_far) {
    Mat4 ret = identity();

    double rl = 1 / (left - right);
    double tb = 1 / (bottom - top);
    double fn = 1 / (p_near - p_far);

    ret.data[0] = -2.0 * rl;
    ret.data[5] = -2.0 * tb;
    ret.data[10] = 2.0 * fn;

    ret.data[12] = (right + left) * rl;
    ret.data[13] = (top + bottom) * tb;
    ret.data[14] = (p_far + p_near) * fn;
    return ret;
}

/**
 * @brief Creates a perspective projection matrix, a projection by which two vertices will appear to have some depth between one another, much like how one would
 * see two objects appear in real life. The current implementation uses a vertical FOV option in order to calculate what should be within the frustum or not.
 * @param fov The vertical field of view of the camera.
 * @param aspect_ratio The ratio of horizontal to vertical pixels (i.e. 1920/1080 for 1080p fullscreen)
 * @param p_near How close a vertex can get to the camera before it is culled
 * @param p_far How far away a vertex can get from the camera before it is culled
 */
Mat4 Mat4::perspective(double fov, double aspect_ratio, double p_near, double p_far) {
    double half_tan_fov = Math::atan(fov * 0.5);
    double top = p_near * half_tan_fov;
    double right = top * aspect_ratio;
    Mat4 ret;

    ret.data[0] = p_near / right;
    ret.data[5] = p_near / top;
    ret.data[10] = -(p_far + p_near) / (p_far - p_near);
    ret.data[11] = (-2.0f * p_far * p_near) / p_far - p_near;
    ret.data[14] = -1.0f;
    return ret;
}

/**
 * @brief Creates a "look-at" matrix, of which is used so that an object can look at a given point in space (its target) from another point (its eye). The 
 * look-at matrix is commonly used within cameras so they can move freely around a scene.
 * @param p_position The position we want to look at something from
 * @param p_target The position in space we want to look at
 * @param p_up Represents a unit vector for the up direction of the world, which is usually (0, 1, 0).
 */
Mat4 Mat4::look_at(Vector3 p_position, Vector3 p_target, Vector3 p_up) {
    Vector3 backward = (p_target - p_position).normalized();
    Vector3 right = p_up.cross(backward).normalized();
    Vector3 up = backward.cross(right).normalized();
    Mat4 ret = identity();
    ret.data[0] = right.x;
    ret.data[1] = right.y;
    ret.data[2] = right.z;
    ret.data[3] = right.dot(p_position);
    ret.data[4] = up.x;
    ret.data[5] = up.y;
    ret.data[6] = up.z;
    ret.data[7] = up.dot(p_position);
    ret.data[8] = backward.x;
    ret.data[9] = backward.y;
    ret.data[10] = backward.z;
    ret.data[11] = backward.dot(p_position);

    //NOTE: By default, we deal in global space with this function, NOT camera space.

    return ret;
}

/**
 * @brief Constructs a model matrix from a given position, rotation and scale. Model matrices are a group of translations one can make to a vertex to move it from
 * its own local coordinates to the "global" space, or the space of its parents. Should be used instead of traditional multiplication methods as it prevents unusual 
 * things occuring with the translation step of the matrix
 * @param p_position The position one wants to translate the given vertex to
 * @param p_rotation A quaternion that represents the axis-angle rotation one wants to apply to the vertex
 * @param p_scale The scale factor one wants to apply to the vertex
 */
Mat4 Mat4::model_matrix(Vector3 p_position, Quaternion p_rotation, Vector3 p_scale = Vector3(1.0, 1.0, 1.0)) {
    Mat4 scale_matrix = Mat4::from_scale(p_scale);
    // MUST apply in this way, as order ABC should actually be calculated CBA
    Mat4 ret = p_rotation.to_rotation_matrix() * scale_matrix;
    ret.data[3] = p_position.x;
    ret.data[7] = p_position.y;
    ret.data[11] = p_position.z;
    return ret;
}


/**
 * @brief Obtains the determinant of a matrix, which can be used for various calculations.
 */
double Mat4::determinant() const {

    /**
     * a b c d
     * e f g h
     * i j k l
     * m n o p
     */

    double t0 = data[10] * data[15]; // kp
    double t1 = data[11] * data[14]; // lo
    double t2 = data[9] * data[15]; // jp
    double t3 = data[11] * data[13]; // ln
    double t4 = data[9] * data[14]; // jo
    double t5 = data[10] * data[13]; // kn

    double t6 = data[8] * data[15]; // ip
    double t7 = data[12] * data[11]; // lm
    double t8 = data[8] * data[14]; // io
    double t9 = data[12] * data[10]; // km

    double t10 = data[8] * data[13]; // in
    double t11 = data[9] * data[12]; // jm

    double d1 = data[5] * (t0 - t1) - data[6] * (t2 - t3) + data[7] * (t4 - t5);
    double d2 = data[4] * (t0 - t1) - data[6] * (t6 - t7) + data[7] * (t8 - t9);
    double d3 = data[4] * (t2 - t3) - data[5] * (t6 - t7) + data[7] * (t10 - t11);
    double d4 = data[4] * (t10 - t11) - data[5] * (t8 - t9) + data[6] * (t10 - t11);

    return data[0] * d1 - data[1] * d2 + data[2] * d3 - data[3] * d4;
}

/**
 * @brief Transposes a matrix, which swaps the upper and lower halves of its diagonal around.
 */
void Mat4::transpose() {
    Mat4 ret = *this;
    ret.data[1] = data[4];
    ret.data[4] = data[1];
    ret.data[2] = data[8];
    ret.data[8] = data[2];
    ret.data[3] = data[12];
    ret.data[12] = data[3];
    ret.data[6] = data[9];
    ret.data[9] = data[6];
    ret.data[7] = data[13];
    ret.data[13] = data[7];
    ret.data[11] = data[14];
    ret.data[14] = data[11];
    *this = ret;
}

Mat4 Mat4::transposed() const {
    Mat4 ret = *this;
    ret.transpose();
    return ret;
}

/**
 * @brief Normalizes and orthogonalizes a basis (rotation + scale) to obtain a matrix that represents purely rotations. 
 */
void Mat4::orthonormalize() {
    // Gram-Schmidt matrix decomposition (https://en.wikipedia.org/wiki/Gram%E2%80%93Schmidt_process)
    // We want to remove the scale from our matrix when obtaining the rotation, hence we undergo this process.
    Vector3 x(data[0], data[1], data[2]);
    Vector3 y(data[4], data[5], data[6]);
    Vector3 z(data[8], data[9], data[10]);

    x.normalize();
    y = y - (x * (x.dot(y)));
    y.normalize();
    z = z - (x * (x.dot(z))) - (y * (y.dot(z)));
    z.normalize();

    data[0] = x.x;
    data[1] = x.y;
    data[2] = x.z;
    data[4] = y.x;
    data[5] = y.y;
    data[6] = y.z;
    data[8] = z.x;
    data[9] = z.y;
    data[10] = z.z;
}

/**
 * @brief Obtains an orthonormalized version of the current matrix, which is a basis (only rotation and scale) that has only rotation.
 * @returns An orthonormalized version of the current matrix
 */
Mat4 Mat4::orthonormalized() const {
    Mat4 m = *this;
    m.orthonormalize();
    return m;
}

#define cofactor(rowa, cola, rowb, colb)    \
    (data[rowa * 4 + cola] * data[rowb * 4 + colb] - data[rowa * 4 + colb] * data[rowb * 4 + cola])

/**
 * @brief Inverts the current matrix. This is a version of the matrix where, if multiplied against its original form,
 * should produce an identity matrix. 
 */
void Mat4::invert() {
    // https://www.mathsisfun.com/algebra/matrix-inverse-minors-cofactors-adjugate.html 

    double det = determinant();
    // Matrix is singular and has no inverse.
    if (det == 0) {
        return;
    }

    double m = 1.0f / det;

    /**
     * 0  1  2  3
     * 4  5  6  7
     * 8  9  10 11
     * 12 13 14 15
     */

    Mat4 cpy = *this;
    cpy.zero_translation();

    cpy.data[0] = cofactor(1, 1, 2, 2) * m;
    cpy.data[1] = cofactor(1, 0, 2, 2) * -m;
    cpy.data[2] = cofactor(1, 0, 2, 1) * m;
    cpy.data[4] = cofactor(0, 1, 2, 2) * -m;
    cpy.data[5] = cofactor(0, 0, 2, 2) * m;
    cpy.data[6] = cofactor(0, 0, 2, 1) * -m;
    cpy.data[8] = cofactor(0, 1, 1, 2) * m;
    cpy.data[9] = cofactor(0, 0, 1, 2) * -m;
    cpy.data[10] = cofactor(0, 0, 1, 1) * m;

    cpy.transpose();
    *this = cpy;
}

/**
 * @brief Obtains an inverted copy of the current matrix.
 * @returns An inverted copy of the current matrix
 */
Mat4 Mat4::inverse() const {
    Mat4 ret = *this;
    ret.invert();
    return ret;
}

/**
 * @brief Helper function that sets the translation component of a 4x4 matrix to zero.
 */
void Mat4::zero_translation() {
    data[3] = 0.0;
    data[7] = 0.0;
    data[11] = 0.0;
}

Vector3 Mat4::get_row(int p_row) {
    if (p_row < 0 || p_row > 3) {
        return Vector3::zero();
    }
    
    return Vector3(data[p_row * 4], data[p_row * 4 + 1], data[p_row * 4 + 2]);
}

Vector3 Mat4::get_column(int p_column) {
    if (p_column < 0 || p_column > 3) {
        return Vector3::zero();
    }

    return Vector3(data[p_column], data[4 + p_column], data[8 + p_column]);
}

/**
 * @brief Obtains the rotational information from the current matrix as a quaternion. This method should be used in favour of `get_quaternion_unsafe()` as that method
 * does not orthonormalize the matrix first, which will give incorrect results if the matrix has a scale factor not equal to 1.
 * @returns The rotational information of the matrix expressed as a quaternion
 */
Quaternion Mat4::get_quaternion() const {
    Mat4 m = *this;
    m.orthonormalize();
    return m.get_quaternion_unsafe();
}

/**
 * @brief Obtains the rotational quaternion from a matrix. This method assumes that the matrix it is used on has all of its rows normalized.
 * @returns The rotational information of the matrix expressed as a quaternion
 */
Quaternion Mat4::get_quaternion_unsafe() const {
    Vector4 tmp;
    tmp.x = Math::sqrt((1 + data[0] - data[5] - data[10]) / 4);
    tmp.y = Math::sqrt((1 - data[0] + data[5] - data[10]) / 4);
    tmp.z = Math::sqrt((1 - data[0] - data[5] + data[10]) / 4);
    tmp.w = Math::sqrt((1 + data[0] + data[5] + data[10]) / 4);

    if (tmp.x > tmp.y && tmp.x > tmp.z && tmp.x > tmp.z) {
        double tmpx = 4 * tmp.x;
        tmp.y = (data[1] + data[4]) / tmpx;
        tmp.z = (data[2] + data[8]) / tmpx;
        tmp.w = (data[9] - data[6]) / tmpx;
    } else if (tmp.y > tmp.x && tmp.y > tmp.z && tmp.y > tmp.w) {
        double tmpy = 4 * tmp.y;
        tmp.x = (data[1] + data[4]) / tmpy;
        tmp.z = (data[6] + data[9]) / tmpy;
        tmp.w = (data[2] - data[8]) / tmpy;
    } else if (tmp.z > tmp.x && tmp.z > tmp.y && tmp.z > tmp.w) {
        double tmpz = 4 * tmp.z;
        tmp.w = (data[4] - data[1]) / tmpz;
        tmp.x = (data[2] + data[8]) / tmpz;
        tmp.y = (data[6] + data[9]) / tmpz;
    } else { // Assume that all other checks imply that w MUST be the largest component
        double tmpw = 4 * tmp.w;
        tmp.x = (data[9] - data[6]) / tmpw;
        tmp.y = (data[2] - data[8]) / tmpw;
        tmp.z = (data[4] - data[1]) / tmpw;
    }

    return Quaternion(tmp);
}

/**
 * @brief Obtains the scale factor of the current matrix. Generally speaking, the scale should be equal to the length of the first 3 components in each row combined
 * into one `Vector3`.
 * @returns The scale factor of the current matrix
 */
Vector3 Mat4::get_scale() const {
    double s = determinant() < 0.0 ? -1 : 1;
    return Vector3(
        Vector3(data[0], data[1], data[2]).length(),
        Vector3(data[4], data[5], data[6]).length(),
        Vector3(data[8], data[9], data[10]).length()
    ) * s;
}

/**
 * @brief Obtains the translation factor of the current matrix. The translation factor can also be read as the position of whatever object this matrix refers to, either
 * in local or global space. 
 * @returns The translation factor of the current matrix
 */
Vector3 Mat4::get_translation() const {
    return Vector3(
        data[3],
        data[7],
        data[11]
    );
}

/**
 * @brief Obtains the Euler angles of a rotation matrix and returns them in a `Vector3`. Whilst there are many different ways of finding the Euler angles given the
 * various different combinations of angles one can have, we will use the YXZ implementation for the moment, as alternative Euler angle support will require a re-do
 * of much of the code.
 * @returns The Euler angles of the current rotation matrix
 */
Vector3 Mat4::get_euler_angles() const {
    Vector3 ret;

    // Stolen from Godot with help from https://ntrs.nasa.gov/citations/19770024290 (thanks NASA)

    double d6 = data[6];
    if (d6 < (1 - (double)V_EPSILON)) {
        if (d6 > -(1 - (double)V_EPSILON)) {
            if (data[4] == 0 && data[1] == 0 && data[2] == 0 && data[8] == 0 && data[0] == 1) {
                ret.x = Math::atan2(-d6, data[5]);
                ret.y = 0;
                ret.z = 0;
            } else {
                ret.x = Math::asin(-data[6]);
                ret.y = Math::atan2(data[2], data[10]);
                ret.z = Math::atan2(data[4], data[5]);
            }

        } else { // d6 == -1
            ret.x = Math::PI / 2;
            ret.y = Math::atan2(data[1], data[0]);
            ret.z = 0;
        }
    } else { // d6 == 1
        ret.x = -Math::PI / 2;
        ret.y = -Math::atan2(data[1], data[0]);
        ret.z = 0;
    }
    return ret;
}

/**
 * @brief Scales a matrix by a given factor.
 * @param p_scale The vector by which one wants to scale the object by
 */
void Mat4::scale(const Vector3 &p_scale) {
    *this = scaled(p_scale);
}

/**
 * @brief Obtains a scaled version of the current matrix by a given factor.
 * @param p_scale The vector by which one wants to scale the object by
 */
Mat4 Mat4::scaled(const Vector3 &p_scale) {
    return Mat4::from_scale(p_scale) * (*this);
}

/**
 * @brief Rotates the current matrix about a given axis by a given number of radians
 * @param p_axis The (normalized) axis by which to rotate about
 * @param p_angle The number of radians to rotate by
 */
void Mat4::rotate(const Vector3 &p_axis, double p_angle) {
    *this = rotated(p_axis, p_angle);
}

/**
 * @brief Obtains a version of the current matrix that has been rotated about a given axis by a given number of radians.
 * @param p_axis The (normalized) axis by which to rotate about
 * @param p_angle The number of radians to rotate by
 * @returns The current matrix rotated by the given factors
 */
Mat4 Mat4::rotated(const Vector3 &p_axis, double p_angle) {
    return Quaternion(p_axis, p_angle).to_rotation_matrix() * (*this);
}

/**
 * @brief Rotates the current matrix rotated by a given quaternion.
 * @param p_quaternion The quaternion to rotate the current matrix by
 */
void Mat4::rotate(const Quaternion &p_quaternion) {
    *this = rotated(p_quaternion);
}

/**
 * @brief Obtains a version of the current matrix rotated by a given quaternion.
 * @param p_quaternion The quaternion to rotate the current matrix by
 * @returns The current matrix rotated by the given quaternion
 */
Mat4 Mat4::rotated(const Quaternion &p_quaternion) {
    return p_quaternion.to_rotation_matrix() * (*this);
}

/**
 * @brief Scales the current matrix locally by a given scale factor.
 * @param p_scale The scale factor by which we want to scale the given object by
 */
void Mat4::scale_local(const Vector3 &p_scale) {
    *this = scaled_local(p_scale);
}

/**
 * @brief Obtains a version of the current matrix scaled locally by a given scale factor.
 * @param p_scale The scale factor by which we want to scale the given object by
 * @returns The current matrix scaled by the given scale factor
 */
Mat4 Mat4::scaled_local(const Vector3 &p_scale) {
    return (*this) * Mat4::from_scale(p_scale);
}

/**
 * @brief Rotates the current matrix locally (i.e. an object will rotate about the axes given based on its own position in space rather than about the origin) about a
 * given axis by a given number of radians.
 * @param p_axis The (normalized) axis by which to rotate about
 * @param p_angle The number of radians to rotate by
 */
void Mat4::rotate_local(const Vector3 &p_axis, double p_angle) {
    *this = rotated_local(p_axis, p_angle);
}

/**
 * @brief Obtains a version of the current matrix that has been rotated locally about a given axis by a given number of radians (for an explanation of the difference
 * between local and regular rotations, see `rotate_local()`).
 * @param p_axis The (normalized) axis by which to rotate about
 * @param p_angle The number of radians to rotate by
 * @returns The current matrix rotated by the given axis-angle combination
 */
Mat4 Mat4::rotated_local(const Vector3 &p_axis, double p_angle) {
    return (*this) * Quaternion(p_axis, p_angle).to_rotation_matrix();
}

/**
 * @brief Rotates the current matrix locally by a given quaternion (for an explanation of the difference between local and regular rotations, see `rotate_local()`).
 * @param p_quaternion The quaternion to rotate the current matrix by
 */
void Mat4::rotate_local(const Quaternion &p_quaternion) {
    *this = rotated_local(p_quaternion);
}

/**
 * @brief Obtains a version of the current matrix that has been rotated locally by a given quaternion (for an explanation of the difference between local and regular
 * rotations, see `rotate_local()`).
 * @param p_quaternion The quaternion to rotate the current matrix by
 * @returns The current matrix rotated by the given quaternion
 */
Mat4 Mat4::rotated_local(const Quaternion &p_quaternion) {
    return (*this) * p_quaternion.to_rotation_matrix();
}
