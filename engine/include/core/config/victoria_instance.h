#pragma once

#include <core/typedefs.h>

#include "item.h"

class VAPI VictoriaInstance : public Item {
    VREGISTER_CLASS(VictoriaInstance, Item)

    bool started = false;
public:
    bool initialize();
    bool iteration();
    void finalize();

    VictoriaInstance();
    ~VictoriaInstance();
};