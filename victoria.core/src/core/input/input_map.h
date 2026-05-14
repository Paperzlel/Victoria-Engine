#pragma once

#include "core/input/input_event.h"
#include "core/string/vname.h"
#include "core/typedefs.h"

class VAPI InputMap {
	static InputMap *singleton;

	struct Action {
		int id;
		List<Ref<InputEvent>> events;
	};

	HashTable<VName, Action> action_map;
	int total_ids = 0;

public:
	bool add_action(const VName &p_action_name);
	void erase_action(const VName &p_action_name);
	bool has_action(const VName &p_action_name) const;

	Error add_action_event(const VName &p_action_name, const Ref<InputEvent> &p_event);
	void clear_action_events(const VName &p_action_name);

	List<Ref<InputEvent>> *get_action_events(const VName &p_action_name);

	void load_default_actions();

	InputMap();
	~InputMap();
};
