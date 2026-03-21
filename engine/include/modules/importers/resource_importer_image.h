#pragma once

#include "core/io/resource_format_importer.h"
#include "core/typedefs.h"

class ResourceFormatImporterImage : public ResourceFormatImporter {
public:
	virtual Ref<Resource> _import(const String &p_file, int p_argc, Variant *p_args) override;

	ResourceFormatImporterImage();
};