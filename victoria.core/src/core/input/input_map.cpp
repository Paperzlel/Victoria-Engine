#include "core/input/input_map.h"

InputMap *InputMap::singleton = nullptr;

InputMap *InputMap::get_singleton() {
	return singleton;
}

bool InputMap::add_action(const VName &p_action_name) {
	ERR_FAIL_COND_MSG_R(action_map.has(p_action_name), "Input action already exists.", false);

	action_map[p_action_name] = Action();
	action_map[p_action_name].id = total_ids;
	action_map[p_action_name].events = List<Ref<InputEvent>>();
	return true;
}

void InputMap::erase_action(const VName &p_action_name) {
	ERR_FAIL_COND_MSG(!action_map.has(p_action_name), "Input action does not exist.");

	action_map.erase(p_action_name);
}

bool InputMap::has_action(const VName &p_action_name) const {
	return action_map.has(p_action_name);
}

Error InputMap::add_action_event(const VName &p_action_name, const Ref<InputEvent> &p_event) {
	if (p_event.is_null() || p_action_name.is_empty()) {
		return ERR_INVALID_PARAMETER;
	}

	if (!action_map.has(p_action_name)) {
		return ERR_UNAVAILABLE;
	}

	action_map[p_action_name].events.push_back(p_event);
	return OK;
}

void InputMap::clear_action_events(const VName &p_action_name) {
	ERR_FAIL_COND_MSG(!action_map.has(p_action_name), "Input action does not exist.");
	action_map[p_action_name].events.clear();
}

List<Ref<InputEvent>> *InputMap::get_action_events(const VName &p_action_name) {
	ERR_FAIL_COND_MSG_R(!action_map.has(p_action_name), "Input action does not exist.", nullptr);
	return &action_map[p_action_name].events;
}

List<VName> InputMap::get_actions_for_event(const Ref<InputEvent> &p_event) {
	List<VName> ret;

	for (const KeyValue<VName, Action> &e : action_map) {
		for (Ref<InputEvent> event : e.value.events) {
			if (p_event->action_is_equal(event)) {
				ret.push_back(e.key);
			}
		}
	}

	return ret;
}

void InputMap::load_default_actions() {
	// Loads the default input values that are used by core. Other input systems should register their own alongside
	// these.
	add_action("DEBUG_0");
	add_action("DEBUG_1");
	add_action("DEBUG_2");
	add_action("DEBUG_3");
	add_action("DEBUG_4");
	add_action("DEBUG_5");
	add_action("DEBUG_6");
	add_action("DEBUG_7");
	add_action("DEBUG_8");
	add_action("DEBUG_9");
}

InputMap::InputMap() {
	singleton = this;
}

InputMap::~InputMap() {
	singleton = nullptr;
}
