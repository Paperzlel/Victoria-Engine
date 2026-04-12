#include "scene/main/object.h"

#include "scene/main/window.h"

#include <core/error/error_macros.h>

void Object::_propagate_enter_tree() {
	if (data.parent) {
		data.tree = data.parent->data.tree;
	}
	data.is_inside_tree = true;

	data.viewport = Item::cast_to<Viewport>(this);
	if (!data.viewport && data.parent) {
		data.viewport = data.parent->data.viewport;
	}

	notification(NOTIFICATION_ENTER_TREE, true);

	for (Object *child : data.children) {
		child->_propagate_enter_tree();
	}
}

void Object::_propagate_exit_tree() {
	data.is_inside_tree = false;

	data.viewport = nullptr;

	notification(NOTIFICATION_EXIT_TREE, true);

	for (Object *child : data.children) {
		child->_propagate_exit_tree();
	}
}

void Object::queue_free() {
	ERR_COND_NULL_MSG(data.tree, "Cannot queue_free() an object that is not inside the tree.");
	data.tree->queue_delete(this);
}

void Object::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PREDELETE: {
			if (data.parent) {
				data.parent->remove_child(this);
			}

			// Need to clean up child nodes
			// Use cleaner end-to-beginning sort
			while (data.children.size()) {
				Object *c = data.children.back()->get();
				vdelete(c);
			}
		}
	}
}

Object *Object::get_parent() const {
	return data.parent != nullptr ? data.parent : nullptr;
}

Window *Object::get_window() const {
	Window *w = Item::cast_to<Window>(data.viewport);
	if (!w) {
		return data.viewport->get_base_window();
	}
	return w;
}

Object *Object::get_child(int p_index) const {
	ERR_FAIL_COND_R(p_index >= data.children.size(), nullptr);
	return this->data.children.get(p_index);
}

int Object::get_child_count() const {
	return data.children.size();
}

List<Object *> Object::get_children() const {
	return data.children;
}

void Object::add_child(Object *p_child) {
	ERR_COND_NULL(p_child);
	p_child->data.parent = this;
	if (data.tree) {
		p_child->set_tree(data.tree);
	}

	// Set name of children
	if (p_child->get_name().is_empty()) {
		if (data.children.size() == 0) {
			p_child->set_name(p_child->get_class_name());
		} else {
			int count = 0;
			List<Object *>::Element *c = data.children.front();
			while (c) {
				if (c->get()->get_class_name() == p_child->get_class_name()) {
					count++;
				}

				c = c->next();
			}

			p_child->set_name(p_child->get_class_name() + (count > 0 ? itos(count) : ""));
		}
	}

	data.children.push_back(p_child);
	notification(NOTIFICATION_CHILD_ENTERED_TREE);
}

void Object::remove_child(Object *p_child) {
	ERR_COND_NULL(p_child);
	ERR_FAIL_COND(p_child->data.parent != this);

	p_child->set_tree(nullptr);
	bool found = data.children.erase(p_child);
	ERR_FAIL_COND_MSG(
		!found,
		"Child's parent was set to this node, but could not be found in the given node's list of children.");

	p_child->data.parent = nullptr;
	notification(NOTIFICATION_CHILD_EXITED_TREE);
}

String Object::get_name() const {
	return data.name;
}

void Object::set_name(const String &p_name) {
	data.name = p_name;
	data.name.replace(' ', '_');
}

SceneTree *Object::get_tree() const {
	return data.tree;
}

void Object::set_tree(SceneTree *p_tree) {
	data.tree = p_tree;

	if (p_tree) {
		_propagate_enter_tree();
	} else {
		_propagate_exit_tree();
	}
}

Object *Object::get_object_or_null(const String &p_path) const {
	if (!is_inside_tree()) {
		return nullptr;
	}

	bool is_absolute = false;

	if (p_path.begins_with("/")) {
		is_absolute = true;
	}

	if (is_absolute) {
		return get_tree()->root->get_object_or_null(p_path.left(-1));
	}

	int idx = p_path.find("/");
	if (idx > -1) {
		String child_name = p_path.substr(0, idx);
		for (const Object *c : data.children) {
			if (c->get_name() == child_name) {
				// Child found
				return c->get_object_or_null(p_path.substr(idx + 1, p_path.length() - (idx + 1)));
			}
		}
	} else {
		if (p_path.is_empty()) {
			return nullptr;
		}

		for (Object *c : data.children) {
			if (c->get_name() == p_path) {
				return c;
			}
		}
	}

	return nullptr;
}

Object *Object::get_object(const String &p_path) const {
	Object *ret = get_object_or_null(p_path);
	ERR_COND_NULL_MSG_R(ret, "Object was not found.", nullptr);
	return ret;
}

void Object::propagate_notification(int p_what) {
	notification(p_what);

	List<Object *> children = get_children();
	if (children.size() > 0) {
		for (Object *c : children) {
			c->propagate_notification(p_what);
		}
	}
}

Object::Object() {}

Object::~Object() {
	data.children.clear();
	data.parent = nullptr;
}