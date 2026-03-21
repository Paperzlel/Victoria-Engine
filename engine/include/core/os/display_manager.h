#pragma once

#include "core/config/callable_method.h"
#include "core/math/vector2i.h"
#include "core/string/vstring.h"
#include "core/typedefs.h"
#include "core/variant/variant_caster.h"

/**
 * @brief Base display class. This class works with everything to do with windows and window events.
 */
class VAPI DisplayManager {
protected:
	static DisplayManager *singleton;

	// Whether the mouse is focused on the window or not.
	bool mouse_mode = false;

	typedef DisplayManager *(*DisplayMangerCreationFunc)(const String &, const Vector2i &, Error *);

	struct DisplayManagerCreationStruct {
		DisplayMangerCreationFunc func;
		String name;
	};

	static const int MAX_CREATE_FUNCS = 4;
	static int create_func_count;
	static DisplayManagerCreationStruct _create_funcs[MAX_CREATE_FUNCS];

public:
	static DisplayManager *get_singleton();

	static const uint8_t INVALID_WINDOW_ID = -1;

	enum WindowNotification {
		NOTIFICATION_WM_WINDOW_CLOSE,
		NOTIFICATION_WM_MOUSE_ENTER,
		NOTIFICATION_WM_MOUSE_EXIT,
	};

	enum WindowFlags {
		WINDOW_FLAG_DEFAULT = 0,		// Default behaviour
		WINDOW_FLAG_INVISIBLE = 1 << 0, // Don't display the window on creation
	};

	static DisplayManager *create(int p_func_idx, const String &p_renderer, const Vector2i &p_size, Error *r_error);
	static int get_creation_func_count();
	static const String &get_creation_func_name(int p_id);

	static void _notification_callback(WindowNotification notification, uint8_t window_id);

	virtual uint8_t create_window(const String &p_name,
								  uint16_t x,
								  uint16_t y,
								  uint16_t width,
								  uint16_t height,
								  WindowFlags p_flags = WINDOW_FLAG_DEFAULT) = 0;
	virtual void destroy_window(uint8_t p_id) = 0;

	virtual void set_use_vsync(bool p_value) = 0;

	virtual Vector2i get_window_rect() const = 0;
	virtual void set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) = 0;

	virtual void toggle_mouse_mode(bool p_mode) = 0;
	virtual bool get_mouse_mode() const = 0;

	virtual void process_events() = 0;
	virtual void swap_buffers() = 0;

	virtual void finalize() = 0;

	DisplayManager();
};

VARIANT_CAST_ENUM(DisplayManager::WindowNotification);
VARIANT_CAST_ENUM(DisplayManager::WindowFlags);
