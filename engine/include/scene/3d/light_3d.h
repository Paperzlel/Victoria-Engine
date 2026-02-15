#pragma once

#include <core/typedefs.h>

#include "visual_instance_3d.h"

class VAPI Light3D : public VisualInstance3D {
    VREGISTER_CLASS(Light3D, VisualInstance3D)
protected:

    RID light;

    Vector3 colour;
    Vector3 specular;
public:

    RID get_light() const;

    Vector3 get_colour() const;
    void set_colour(const Vector3 &p_colour);

    Vector3 get_specular() const;
    void set_specular(const Vector3 &p_specular);

    Light3D();
    ~Light3D();
};

class VAPI PointLight3D : public Light3D {
    VREGISTER_CLASS(PointLight3D, Light3D)

    float range = 0;
public:

    float get_range() const;
    void set_range(float p_range);

    PointLight3D();
    ~PointLight3D();
};

class VAPI DirectionalLight3D : public Light3D {
    VREGISTER_CLASS(DirectionalLight3D, Light3D)
public:

    DirectionalLight3D();
    ~DirectionalLight3D();
};

class VAPI SpotLight3D : public Light3D {
    VREGISTER_CLASS(SpotLight3D, Light3D)

    float inner_cone;
    float outer_cone;
public:

    float get_inner_cone() const;
    void set_inner_cone(float p_radius);

    float get_outer_cone() const;
    void set_outer_cone(float p_radius);

    SpotLight3D();
    ~SpotLight3D();
};