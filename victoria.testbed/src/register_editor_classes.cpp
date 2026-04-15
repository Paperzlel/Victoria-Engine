#include "register_editor_classes.h"

#include "editor_camera.h"
#include "editor_main.h"

#include <core/object/class_registry.h>

void register_editor_classes() {
	REGISTER_CLASS(Editor);
	REGISTER_CLASS(EditorCamera);
}

void unregister_editor_classes() {}