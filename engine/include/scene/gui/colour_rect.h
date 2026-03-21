#pragma once

#include "object_ui.h"

#include "core/typedefs.h"

class VAPI ColourRect : public ObjectUI {
	VREGISTER_CLASS(ColourRect, ObjectUI)

	Vector4 colour;

protected:
	void _notification(int p_what);

public:
	Vector4 get_colour() const;
	void set_colour(const Vector4 &p_colour);

	ColourRect() {}
};