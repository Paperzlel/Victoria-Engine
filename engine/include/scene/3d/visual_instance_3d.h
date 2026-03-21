#pragma once

#include "object_3d.h"

#include "core/data/rid.h"
#include "core/typedefs.h"

class VAPI VisualInstance3D : public Object3D {
	VREGISTER_CLASS(VisualInstance3D, Object3D)

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