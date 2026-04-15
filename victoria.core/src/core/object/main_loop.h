#pragma once

#include "core/object/object.h"

class VAPI MainLoop : public Object {
	VREGISTER_CLASS(MainLoop, Object);

public:
	virtual void initialize();
	virtual void update(double p_delta);
	virtual void finalize();
};