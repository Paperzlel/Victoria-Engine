#pragma once

#include "container.h"

#include "core/typedefs.h"

class VAPI VBoxContainer : public Container {
	VREGISTER_CLASS(VBoxContainer, Container)

	void _resize();

protected:
	void _notification(int p_what);

public:
	VBoxContainer();
};
