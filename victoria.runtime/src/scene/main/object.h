#pragma once

#include "scene/main/scene_tree.h"

#include <core/config/item.h>
#include <core/data/list.h>
#include <core/string/vstring.h>

class Viewport;
class Window;

/**
 * This class represents a single "node" on the scene tree.
 */
class VAPI Object : public Item {
	VREGISTER_CLASS(Object, Item)

	struct Data {
		List<Object *> children;
		Object *parent = nullptr;
		Viewport *viewport = nullptr;

		bool is_inside_tree = false;
		String name = "";

		SceneTree *tree = nullptr;
	} data;

	friend class SceneTree;

	void _propagate_enter_tree();
	void _propagate_exit_tree();

	void set_tree(SceneTree *p_tree);

protected:
	void _notification(int p_what);

public:
	void queue_free();

	Object *get_parent() const;

	FORCE_INLINE Viewport *get_viewport() const {
		return data.viewport;
	}
	Window *get_window() const;

	Object *get_child(int p_index) const;
	List<Object *> get_children() const;
	int get_child_count() const;
	void add_child(Object *p_child);
	void remove_child(Object *p_child);

	String get_name() const;
	void set_name(const String &p_name);

	SceneTree *get_tree() const;
	FORCE_INLINE bool is_inside_tree() const {
		return data.is_inside_tree;
	}

	Object *get_object_or_null(const String &p_path) const;

	Object *get_object(const String &p_path) const;

	void propagate_notification(int p_what);

	Object();
	~Object();
};
