#pragma once

#include <core/data/list.h>
#include <core/object/main_loop.h>

#ifdef Window
#	undef Window
#endif

class GameObject;
class Window;
class Camera3D;

class VAPI SceneTree : public MainLoop {
	VREGISTER_CLASS(SceneTree, MainLoop);

private:
	static SceneTree *singleton;
	friend class GameObject;

	Window *root = nullptr;
	Camera3D *active_camera = nullptr;
	double update_time = 0.0;

	List<GameObject *> queued_nodes_for_deletion;

public:
	static SceneTree *get_singleton();

	FORCE_INLINE Window *get_root() const {
		return root;
	}

	void queue_delete(GameObject *p_object);
	void flush_delete_queue();

	double get_update_time() const;

	void propagate_tree_notification(int p_what);

	friend class Camera3D;
	void set_active_camera(Camera3D *p_camera);
	Camera3D *get_active_camera();

	virtual void initialize() override;
	virtual void update(double p_delta) override;
	virtual void finalize() override;

	SceneTree();
	~SceneTree();
};
