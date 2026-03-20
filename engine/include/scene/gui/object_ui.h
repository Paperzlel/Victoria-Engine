#pragma once

#include "core/typedefs.h"

#include "scene/main/canvas_item.h"
#include "core/variant/variant_caster.h"

class VAPI ObjectUI : public CanvasItem {
	VREGISTER_CLASS(ObjectUI, CanvasItem)
public:

	enum Anchor {
		ANCHOR_CENTERED,
		ANCHOR_TOP_LEFT,
		ANCHOR_TOP_MIDDLE,
		ANCHOR_TOP_RIGHT,
		ANCHOR_MIDDLE_LEFT,
		ANCHOR_MIDDLE_RIGHT,
		ANCHOR_BOTTOM_LEFT,
		ANCHOR_BOTTOM_MIDDLE,
		ANCHOR_BOTTOM_RIGHT,
	};

	enum Axis {
		AXIS_X,
		AXIS_Y
	};

private:

	struct Data {
		Transform2D transform;
		Vector2i size = Vector2i(40, 40);
		Vector2i min_size = Vector2i(40, 40);

		Vector2i position;		// Actual position of the object
		Vector2i pos_cached;	// Cached position for when we change size but don't want to update yet
		Vector2i size_cached;	// Cached size for above reasons

		double anchor_factor[2] = {0.0, 0.0};		// X/Y factor for each anchor

		ObjectUI *ui_parent = nullptr;
	} data;

	void _rect_changed();
	void _update_minimum_size();
	void _update_anchors(Anchor p_anchor, bool p_keep_position = false);
	void _update_anchor(Axis p_axis, double p_factor, bool p_keep_position);

	void _update_canvas_item_transform();
public:

	virtual Transform2D get_transform() const override;
	void set_transform(const Transform2D &p_transform);

	Vector2i get_position() const;
	void set_position(const Vector2i &p_pos);

	double get_rotation() const;
	void set_rotation(double p_angle);

	Vector2 get_scale() const;
	void set_scale(const Vector2 &p_scale);
protected:

	Anchor anchor_location = ANCHOR_TOP_LEFT;

	void _notification(int p_what);

	static void _bind_methods();
public:

	Anchor get_anchor_location() const;
	void set_anchor_location(Anchor p_location);

	Vector2i get_size() const;
	void set_size(const Vector2i &p_size);

	Vector2i get_parent_rect() const;

	ObjectUI() {}
	~ObjectUI() {}
};

VARIANT_CAST_ENUM(ObjectUI::Anchor);
VARIANT_CAST_ENUM(ObjectUI::Axis);