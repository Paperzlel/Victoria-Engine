#pragma once

#include "scene/gui/ui_object.h"

class VAPI ColourRect : public UIObject {
	VREGISTER_CLASS(ColourRect, UIObject);

	Vector4 colour;

protected:
	void _notification(int p_what);

public:
	Vector4 get_colour() const;
	void set_colour(const Vector4 &p_colour);

	ColourRect() {}
};