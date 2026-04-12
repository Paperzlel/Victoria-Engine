#pragma once

#include "item.h"

class VAPI MainLoop : public Item {
	VREGISTER_CLASS(MainLoop, Item);

public:
	virtual void initialize();
	virtual void update(double p_delta);
	virtual void finalize();
};