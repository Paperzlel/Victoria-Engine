#pragma once

#include <core/data/rid.h>
#include <core/math/mat4.h>

#include "object_3d.h"

class VAPI Camera3D : public Object3D {
    VREGISTER_CLASS(Camera3D, Object3D)

    float fovy = 70.0 * Math::DEG_TO_RAD_MULTIPLIER;
    float near_plane = 0.1;
    float far_plane = 1000.0;
    bool is_active_camera = false;

    RID camera;

    void _update_camera();
protected:

    friend class Viewport;
    void _notification(int p_what);
public:

    friend class SceneTree;

    void toggle_active(bool p_active);
    bool is_active();

    Transform3D get_camera_transform() const;
    Mat4 get_projection() const;

    RID get_camera() const;

    float get_fovy() const;
    void set_fovy(float p_fov);

    float get_near() const;
    void set_near(float p_near);

    float get_far() const;
    void set_far(float p_far);

    Camera3D();
    ~Camera3D();
};
