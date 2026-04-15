#pragma once

#include "scene/gui/ui_object.h"

class VAPI Panel : public UIObject {
	VREGISTER_CLASS(Panel, UIObject);

	Vector4 colour;

protected:
	void _notification(int p_what);

public:
	Vector4 get_colour() const;
	void set_colour(const Vector4 &p_colour);

	Panel() {}
	~Panel() {}
};