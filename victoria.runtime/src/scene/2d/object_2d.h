#pragma once

#include "scene/main/canvas_item.h"

#include <core/math/transform_2d.h>

class VAPI Object2D : public CanvasItem {
	VREGISTER_CLASS(Object2D, CanvasItem)

	Transform2D transform;

	double rotation = 0;
	Vector2 t_scale = Vector2::one();

	void _update_transform();

public:
	void _notification(int p_what);

	Vector2 get_position() const;
	double get_rotation() const;
	Vector2 get_scale() const;

	void set_position(const Vector2 &p_position);
	void set_rotation(double p_rotation);
	void set_scale(const Vector2 &p_scale);

	Vector2 get_global_position() const;
	double get_global_rotation() const;
	Vector2 get_global_scale() const;

	void set_global_position(const Vector2 &p_position);
	void set_global_rotation(double p_rotation);
	void set_global_scale(const Vector2 &p_scale);

	void translate(const Vector2 &p_translation);
	void rotate(double p_rotation);
	void scale(const Vector2 &p_scale);

	void translate_global(const Vector2 &p_translation);
	void rotate_global(double p_rotation);
	void scale_global(const Vector2 &p_scale);

	virtual Transform2D get_transform() const override;

	void set_transform(const Transform2D &p_transform);
	void set_global_transform(const Transform2D &p_transform);

	Object2D();
	~Object2D();
};