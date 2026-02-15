#pragma once

#include <core/typedefs.h>
#include <core/math/vector2.h>

#include <scene/3d/camera_3d.h>

class EditorCamera : public Camera3D {
    VREGISTER_CLASS(EditorCamera, Camera3D)

    Vector3 direction;
    Vector3 o_position;
    Vector2 offset;
    double speed;

    bool first_frame = true;

    void _update(double p_delta);
public:


    void _notification(int p_what);

    EditorCamera();
    ~EditorCamera();
};