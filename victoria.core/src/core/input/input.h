#pragma once

#include "core/data/hashtable.h"
#include "core/data/rb_set.h"
#include "core/input/input_event.h"
#include "core/input/keyboard.h"
#include "core/math/vector2i.h"
#include "core/typedefs.h"
#include "core/variant/variant_caster.h"

/**
 * @brief Input singleton class. Handles raw and emulated input events and is the primary location input data can be
 * found. For the events themselves, see `InputEvent`. For the mapping of inputs and their events, see `InputMap`. For
 * the enumerations that are used by the class, see `InputEnums`.
 */
class VAPI Input {
	static Input *singleton;

	// Set of pressed and released keys.
	RBSet<Key> pressed_keys;

	// Mask for whether a given mouse button (left, right, or middle) is pressed.
	enum MouseButtonMask {
		MBM_LEFT = 1 << 0,	 // Left mouse button is pressed.
		MBM_RIGHT = 1 << 1,	 // Right mouse button is pressed.
		MBM_MIDDLE = 1 << 2, // Middle mouse button is pressed.
	};

	// Information relating to the mouse's current position on the screen.
	struct MouseInfo {
		uint8_t button_mask;	  // Input mask for what buttons are currently pressed.
		int8_t scroll;			  // Whether the scroll wheel is being moved. -1 is down, 1 is up.
		Vector2i position;		  // The position of the cursor relative to the main screen.
		Vector2i relative_motion; // The relative motion of the cursor from its initial position.
	} mouse_info;

	// Cached data relating to a given input action.
	struct ActionCache {
		uint64_t engine_frame = 0; // The "engine frame" the event occured on. See `Engine::get_frame_count`.
		bool pressed = false;	   // Whether the given action was pressed or not.
	};

	// A table of each input action and their corresponding information.
	HashTable<VName, ActionCache> action_cache;

public:
	typedef void (*PFN_InputEventDispatchFunc)(const Ref<InputEvent> &);

private:
	// The input event callback to use.
	PFN_InputEventDispatchFunc input_event_callback = nullptr;

	/**
	 * @brief Converts the mouse button to a button mask.
	 * @param p_button The button to convert.
	 * @return A button mask for a given input.
	 */
	uint8_t _get_mask_from_button(InputEnums::MouseButton p_button);

public:
	/**
	 * @brief Gets the singleton for this class.
	 * @return The `Input` singleton.
	 */
	static Input *get_singleton();

	// Input actions

	/**
	 * @brief Checks to see if a given action has been pressed by the user.
	 * @param p_action_name The action to check for.
	 * @return `true` if it is pressed, `false` if not.
	 */
	bool is_action_pressed(const VName &p_action_name);

	/**
	 * @brief Checks to see if a given action is currently not being pressed down.
	 * @param p_action_name The action to check for.
	 * @return `true` if released, `false` if pressed. If the action does not exist, it assumes that it has not been
	 * pressed.
	 */
	bool is_action_released(const VName &p_action_name);

	/**
	 * @brief Checks to see if an action was pressed on the same process frame as the callee.
	 * @param p_action_name The action to check for.
	 * @return `true` if pressed on the same process frame, `false` if not.
	 */
	bool is_action_just_pressed(const VName &p_action_name);

	/**
	 * @brief Checks to see if an action was released on the same process frame as the callee.
	 * @param p_action_name The action to check for.
	 * @return `true` if released on the same process frame, `false` if not. If the action does not exist, it assumes
	 * that it has not been pressed.
	 */
	bool is_action_just_released(const VName &p_action_name);

	// Key presses/releases

	/**
	 * @brief Checks if the given key is currently being held down by the user.
	 */
	bool is_key_pressed(Key p_key);

	/**
	 * @brief Checks if the given key is not being held down by the user.
	 */
	bool is_key_released(Key p_key);

	// Mouse button presses/releases

	/**
	 * @brief Checks if the given mouse button is currently being pressed down by the user.
	 */
	bool is_mouse_button_pressed(InputEnums::MouseButton p_button);

	/**
	 * @brief Checks if the given mouse button is not being held down by the user.
	 */
	bool is_mouse_button_released(InputEnums::MouseButton p_button);

	// Mouse wheel movements

	/**
	 * @brief Gets the scroll direction of the mouse wheel. -1 is downwards, 0 is netural, and 1 is upwards.
	 */
	int8_t get_mouse_scroll() const {
		return mouse_info.scroll;
	}

	// Mouse movement commands

	/**
	 * @brief Gets the current position of the mouse in absolute screen coordinates, with (0, 0) being in the
	 * top-leftmost corner in the Windows implementation.
	 */
	Vector2i get_mouse_screen_position() const {
		return mouse_info.position;
	}

	/**
	 * @brief Returns the relative (the change in x/y position) position of the cursor on the screen. The general
	 * origin for the given cursor is calculated as the centre of the screen, and any movements are then limited to
	 * whether the cursor is captured or not.
	 */
	Vector2i get_mouse_motion();

	/**
	 * @brief Gets a `Vector2` that represents a horizontal and vertical vector in relation to whether the given keys
	 * were pressed.
	 * @param p_left The -X axis
	 * @param p_right The +X axis
	 * @param p_up The +Y axis
	 * @param p_down The -Y axis
	 */
	Vector2 get_vector(Key p_left, Key p_right, Key p_up, Key p_down);

	// Event processing

	/**
	 * @brief Translates an `InputEvent` into its inputs and updates the system state in the process. Should not be
	 * used by most end-users except when required.
	 * @param p_event The event in question to send.
	 */
	void parse_input_event(const Ref<InputEvent> &p_event);

	/**
	 * @brief Sets the input event dispatch function to the given argument. The function must have 1 parameter of type
	 * `const Ref<InputEvent>`.
	 * @param p_input_callback The input dispatch function to set it to.
	 */
	void set_input_event_callback(PFN_InputEventDispatchFunc p_input_callback);

	/**
	 * @brief Unpresses any inputs currently held manually. Allows the input system to properly clear itself if focus
	 * is lost.
	 */
	void clear();

	/**
	 * @brief Updates the input state by clearing required data.
	 */
	void update();

	/**
	 * @brief Input singleton class constructor.
	 *
	 */
	Input();

	/**
	 * @brief Input singleton class destructor.
	 *
	 */
	~Input();
};

VARIANT_CAST_ENUM(Key);
VARIANT_CAST_ENUM(InputEnums::MouseButton);
