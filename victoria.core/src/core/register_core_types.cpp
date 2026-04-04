#include "core/register_core_types.h"

#include "core/config/class_registry.h"
#include "core/config/instance.h"
#include "core/config/item.h"
#include "core/io/filesystem.h"
#include "core/io/resource.h"
#include "core/io/resource_format_importer.h"

void register_core_types() {
	REGISTER_CLASS(Item);
	REGISTER_CLASS(Instance);
	REGISTER_CLASS(Resource);
	REGISTER_CLASS(ResourceFormatImporter);
	REGISTER_CLASS(FileSystem);
}

void unregister_core_types() {}