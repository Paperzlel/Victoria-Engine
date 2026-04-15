#include "core/register_core_types.h"

#include "core/io/filesystem.h"
#include "core/io/resource.h"
#include "core/io/resource_format_importer.h"
#include "core/object/class_registry.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"

void register_core_types() {
	REGISTER_CLASS(Object);
	REGISTER_CLASS(RefCounted);
	REGISTER_CLASS(Resource);
	REGISTER_CLASS(ResourceFormatImporter);
	REGISTER_CLASS(FileSystem);
}

void unregister_core_types() {}