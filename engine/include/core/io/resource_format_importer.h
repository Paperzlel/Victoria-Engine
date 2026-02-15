#pragma once

#include <core/typedefs.h>
#include <core/data/list.h>

#include "resource.h"

class VAPI ResourceFormatImporter : public Instance {
protected:
    List<String> supported_formats;

public:
    virtual Ref<Resource> _import(const String &p_path, int p_argc, Variant *p_args) { return Ref<Resource>(); }
    List<String> get_extension_list() { return supported_formats; }
};
