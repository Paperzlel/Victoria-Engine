#include "core/register_core_types.h"

#include "core/engine.h"
#include "core/input/input_event.h"
#include "core/io/file_handle.h"
#include "core/io/resource.h"
#include "core/io/resource_format_importer.h"
#include "core/object/class_registry.h"
#include "core/object/object.h"
#include "core/object/ref_counted.h"
#include "core/string/vname.h"

void register_core_types() {
	VName::setup();

	REGISTER_CLASS(Object);
	REGISTER_CLASS(RefCounted);
	REGISTER_CLASS(Resource);

	REGISTER_ABSTRACT_CLASS(InputEvent);
	REGISTER_CLASS(InputEventKey);
	REGISTER_ABSTRACT_CLASS(InputEventMouse);
	REGISTER_CLASS(InputEventMouseButton);
	REGISTER_CLASS(InputEventMouseMotion);
	REGISTER_CLASS(InputEventMouseScroll);

	REGISTER_CLASS(ResourceFormatImporter);
	REGISTER_ABSTRACT_CLASS(FileHandle);
	REGISTER_CLASS(Engine);
}

void unregister_core_types() {
	ClassRegistry::cleanup();
	VName::finalize();
}