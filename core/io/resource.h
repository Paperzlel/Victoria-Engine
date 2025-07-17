#pragma once

#include "core/typedefs.h"
#include "core/config/instance.h"


class Resource : public Instance {
    VREGISTER_CLASS(Resource, Instance)

    String name = "";
    String path = "";
public:
    
    String get_name() const;
    void set_name(const String &p_name);

    String get_path() const;
    void set_path(const String &p_path);

    Resource() {}
    ~Resource() {}
};
