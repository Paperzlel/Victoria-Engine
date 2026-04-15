#pragma once

#include "scene/3d/object_3d.h"

#include <core/data/rid.h>

class VAPI VisualInstance3D : public GameObject3D {
	VREGISTER_CLASS(VisualInstance3D, GameObject3D);

	RID instance;
	RID base;

protected:
	void _notification(int p_what);

public:
	RID get_instance() const;
	// No setter since we control our own instances

	RID get_base() const;
	void set_base(RID p_base);

	VisualInstance3D();
	~VisualInstance3D();
};