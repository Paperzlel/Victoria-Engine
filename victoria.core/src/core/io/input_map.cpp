#include "core/io/input_map.h"

InputMap *InputMap::singleton = nullptr;

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

void InputMap::load_default_actions() {
	// Loads the default input values that are used by core. Other input systems should register their own alongside
	// these.
	add_action("ENGINE_debug_0");
	add_action("ENGINE_debug_1");
	add_action("ENGINE_debug_2");
	add_action("ENGINE_debug_3");
	add_action("ENGINE_debug_4");
	add_action("ENGINE_debug_5");
	add_action("ENGINE_debug_6");
	add_action("ENGINE_debug_7");
	add_action("ENGINE_debug_8");
	add_action("ENGINE_debug_9");
}

InputMap::InputMap() {
	singleton = this;
}

InputMap::~InputMap() {
	singleton = nullptr;
}
