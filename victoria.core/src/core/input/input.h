#pragma once

#include "core/data/hashtable.h"
#include "core/data/rb_set.h"
#include "core/input/input_event.h"
#include "core/input/keyboard.h"
#include "core/math/vector2i.h"
#include "core/typedefs.h"
#include "core/variant/variant_caster.h"

/**
 * @brief Input singleton class.
 */
class VAPI Input {
	static Input *singleton;

	RBSet<Key> pressed_keys;

	enum MouseButtonMask {
		MBM_LEFT = 1 << 0,
		MBM_RIGHT = 1 << 1,
		MBM_MIDDLE = 1 << 2,
	};

	struct MouseInfo {
		uint8_t button_mask;
		int8_t scroll;
		Vector2i position;
		Vector2i relative_position;
	};

	MouseInfo mouse_info;

	struct ActionCache {
		uint64_t engine_frame = 0;
		bool pressed = false;
	};

	HashTable<VName, ActionCache> action_cache;

	uint8_t _get_mask_from_button(MouseButton p_button);

public:
	static Input *get_singleton();

	// Key presses/releases

	bool is_key_pressed(Key p_key);
	bool is_key_just_pressed(Key p_key);
	bool is_key_released(Key p_key);
	bool is_key_just_released(Key p_key);

	// Mouse button presses/releases

	bool is_mouse_button_pressed(MouseButton p_button);
	bool is_mouse_button_just_pressed(MouseButton p_button);
	bool is_mouse_button_released(MouseButton p_button);
	bool is_mouse_button_just_released(MouseButton p_button);

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

	Vector2i get_mouse_motion();

	Vector2 get_vector(Key p_left, Key p_right, Key p_up, Key p_down);

	// Event processing

	void parse_input_event(const Ref<InputEvent> &p_event);
	void clear();

	void update();
	Input();
};

VARIANT_CAST_ENUM(Key);
VARIANT_CAST_ENUM(MouseButton);
