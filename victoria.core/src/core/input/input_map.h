#pragma once

#include "core/input/input_event.h"
#include "core/string/vname.h"
#include "core/typedefs.h"

/**
 * @brief Input map class. Handles the various inputs names that are created by the user (and the engine) which
 * correspond to specific `InputEvents`.
 */
class VAPI InputMap {
	static InputMap *singleton; // Singleton instance of the class.

	// An "action" is a series of inputs that a given name looks up. IDs are stored to ensure that no two actions are
	// the same.
	struct Action {
		int id;						  // The ID of the action
		List<Ref<InputEvent>> events; // The events correlating to the given action.
	};

	HashTable<VName, Action> action_map; // A map of all the actions that exist.
	int total_ids = 0;					 // The total number of IDs allocated.

public:
	/**
	 * @brief Gets the `InputMap` singleton.
	 */
	static InputMap *get_singleton();

	/**
	 * @brief Adds an action to the action map, if it doesn't already exist.
	 * @param p_action_name The name of the action to create.
	 * @return `true` on success, and `false` if the action already exists.
	 */
	bool add_action(const VName &p_action_name);

	/**
	 * @brief Erases the given action from the action map, if it does in fact exist.
	 * @param p_action_name The name of the action to erase.
	 */
	void erase_action(const VName &p_action_name);

	/**
	 * @brief Checks to see if the given action exists in the action map.
	 * @param p_action_name The name of the action to look for.
	 * @return `true` if yes, `false` if not.
	 */
	bool has_action(const VName &p_action_name) const;

	/**
	 * @brief Adds an action event to the corresponding action in the map. The action must be a valid `InputEvent` or
	 * the action will not be added. Additionally, the input action must have been created with the `add_action` method
	 * or the engine will not be able to add the given action.
	 * @param p_action_name The name of the action to add an event to.
	 * @param p_event The `InputEvent` to add to the action.
	 * @return `OK` on success, `ERR_INVALID_PARAMETER` if the name of the action is empty or if the `InputEvent` is
	 * null, and `ERR_DOESNT_EXIST` if the input action is not already in the action map.
	 */
	Error add_action_event(const VName &p_action_name, const Ref<InputEvent> &p_event);

	/**
	 * @brief Clears all input events relating to the given input action from the map.
	 * @param p_action_name The name of a valid input action.
	 */
	void clear_action_events(const VName &p_action_name);

	/**
	 * @brief Gets a pointer to a list of all valid input events correlating to a given action. It is inadvisable to
	 * use this class outside of very specific scenarios.
	 * @param p_action_name The name of the action to get the event list of.
	 * @return A constant pointer to a list of `InputEvent`s.
	 */
	const List<Ref<InputEvent>> *get_action_events(const VName &p_action_name) const;

	/**
	 * @brief Gets a series of input actions that correlate with a specific input event, assuming the input event is
	 * valid.
	 * @param p_event The event to find the correlating input actions of.
	 * @return A list of input actions for the given event.
	 */
	List<VName> get_actions_for_event(const Ref<InputEvent> &p_event);

	/**
	 * @brief Loads the default actions available to the engine. Currently only used for debugging events. It is
	 * inadvisable to call this event outside of `core`.
	 */
	void load_default_actions();

	InputMap();
	~InputMap();
};
