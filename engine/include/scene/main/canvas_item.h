#pragma once

#include <core/typedefs.h>
#include <core/io/resource.h>
#include <scene/resources/mesh.h>
#include <core/math/transform_2d.h>

#include "object.h"

class VAPI CanvasItem : public Object {
    VREGISTER_CLASS(CanvasItem, Object)

    RID item;

    mutable Transform2D global_transform;
    bool transforms_dirty = true;

    bool skip_draw = false;
    int ysort = 0;
protected:

    void _set_transforms_dirty();
    void _propagate_transform_changed(CanvasItem *p_canvas_item);

    void _notification(int p_what);
public:

    RID get_canvas_item() const;

    Transform2D get_global_transform() const;

    virtual Transform2D get_transform() const = 0;

    void force_redraw();

    void canvas_set_colour(const Vector4 &p_colour);
    void canvas_set_rect(const Vector2 &p_position, const Vector2 &p_size);
    void canvas_set_texture_rect(const Ref<Texture> &p_tex, const Vector2 &p_position, const Vector2 &p_size);
    void canvas_set_mesh(const Ref<Mesh> &p_mesh);
    void canvas_flip_y(bool p_value);
    
    void canvas_set_material(const Ref<Material> &p_material);

    int get_ysort() const;
    void set_ysort(int p_value);

    bool get_skip_draw() const;
    void set_skip_draw(bool p_value);

    CanvasItem();
    ~CanvasItem();
};