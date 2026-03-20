#pragma once

#include "core/typedefs.h"
#include "core/data/vector.h"

#include "resource.h"
#include "resource_format_importer.h"

class VAPI ResourceImporter {
private:

    Vector<Ref<ResourceFormatImporter>> importers;
protected:

    static ResourceImporter *singleton;
public:

    static ResourceImporter *get_singleton();

    enum ImportFormatType {
        IMPORT_FORMAT_NONE,
        IMPORT_FORMAT_PNG,
        IMPORT_FORMAT_JPEG,
        IMPORT_FORMAT_OBJ,
    };

    void add_importer(Ref<ResourceFormatImporter> p_importer);

    Ref<Resource> import(const String &p_path, int p_argc = 0, Variant *p_args = nullptr, Error *r_error = nullptr);

    ResourceImporter();
};