#pragma once

#include "scene/gui/object_ui.h"

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