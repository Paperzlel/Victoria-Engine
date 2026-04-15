#include "scene/register_scene_classes.h"

#include "scene/2d/game_object_2d.h"
#include "scene/2d/mesh_2d.h"
#include "scene/2d/sprite_2d.h"
#include "scene/3d/camera_3d.h"
#include "scene/3d/game_object_3d.h"
#include "scene/3d/light_3d.h"
#include "scene/3d/mesh_3d.h"
#include "scene/3d/visual_instance_3d.h"
#include "scene/gui/colour_rect.h"
#include "scene/gui/container.h"
#include "scene/gui/panel.h"
#include "scene/gui/text.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/ui_object.h"
#include "scene/gui/viewport_container.h"
#include "scene/main/canvas_item.h"
#include "scene/main/game_object.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"
#include "scene/main/window.h"
#include "scene/resources/font.h"
#include "scene/resources/material.h"
#include "scene/resources/mesh.h"
#include "scene/resources/texture.h"

#include <core/io/resource.h>
#include <core/object/class_registry.h>

void register_scene_classes() {
	REGISTER_CLASS(GameObject);
	REGISTER_CLASS(SceneTree);
	REGISTER_ABSTRACT_CLASS(CanvasItem);
	REGISTER_CLASS(Viewport);
	REGISTER_CLASS(Window);

	REGISTER_CLASS(Resource);
	REGISTER_CLASS(Font);
	REGISTER_CLASS(Material);
	REGISTER_CLASS(Mesh);
	REGISTER_CLASS(Texture);
	REGISTER_CLASS(ViewportTexture);

	REGISTER_CLASS(UIObject);
	REGISTER_CLASS(ColourRect);
	REGISTER_CLASS(TextureRect);
	REGISTER_CLASS(Panel);
	REGISTER_CLASS(Text);
	REGISTER_CLASS(Container);
	REGISTER_CLASS(ViewportContainer);

	REGISTER_CLASS(GameObject2D);
	REGISTER_CLASS(Mesh2D);
	REGISTER_CLASS(Sprite2D);

	REGISTER_CLASS(GameObject3D);
	REGISTER_CLASS(Camera3D);
	REGISTER_CLASS(VisualInstance3D);
	REGISTER_CLASS(Mesh3D);
	REGISTER_CLASS(Light3D);
}

void unregister_scene_classes() {}