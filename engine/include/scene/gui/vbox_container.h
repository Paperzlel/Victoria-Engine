#pragma once

#include <core/typedefs.h>

#include "container.h"

class VAPI VBoxContainer: public Container {
    VREGISTER_CLASS(VBoxContainer, Container)

    void _resize();
protected:

    void _notification(int p_what);
public:

    VBoxContainer();
};
