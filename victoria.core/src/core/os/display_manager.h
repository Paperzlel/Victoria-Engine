#pragma once

#include "core/math/vector2i.h"
#include "core/object/callable_method.h"
#include "core/string/vstring.h"
#include "core/typedefs.h"
#include "core/variant/variant_caster.h"

/**
 * @brief Base display manager. This class works with everything to do with the desktop environment, including
 * keyboards + mice, tablets, clipboards, TTS, and windows.
 */
class VCORE_API DisplayManager {
protected:
	static DisplayManager *singleton;

	typedef DisplayManager *(*DisplayMangerCreationFunc)(const String &, const Vector2i &, Error *);

	struct DisplayManagerCreationStruct {
		DisplayMangerCreationFunc func;
		const char *name;
	};

	static const int MAX_CREATE_FUNCS = 4;
	static int create_func_count;
	static DisplayManagerCreationStruct _create_funcs[MAX_CREATE_FUNCS];

	// Whether the mouse is focused on the window or not.
	bool mouse_mode = false;

public:
	static DisplayManager *get_singleton();

	// The ID for an invalid window.
	static const uint8_t INVALID_WINDOW_ID = -1;

	// All notifications available to a window. Should be aligned with the `Object` notifications as well.
	enum WindowNotification {
		NOTIFICATION_WM_WINDOW_CLOSE, // Closing window event
		NOTIFICATION_WM_MOUSE_ENTER,  // Mouse entering window event
		NOTIFICATION_WM_MOUSE_EXIT,	  // Mouse exiting window event
	};

	enum WindowFlags {
		WINDOW_FLAG_DEFAULT = 0,		// Default behaviour
		WINDOW_FLAG_INVISIBLE = 1 << 0, // Don't display the window on creation
	};

	/**
	 * @brief Creates a new `DisplayManager` singleton. Should only be called once for an ideally obvious reason.
	 * @param p_func_idx The ID for the creation function to call.
	 * @param p_renderer The rendering backend to use.
	 * @param p_size The size of the root window.
	 * @param r_error The error code to return after creation. Optional, but is highly recommended.
	 * @return DisplayManager*
	 */
	static DisplayManager *create(int p_func_idx, const String &p_renderer, const Vector2i &p_size, Error *r_error);

	/**
	 * @brief Gets the number of registered creation functions.
	 * @return The number of registered creation functions. Will always be at least once since a headless driver
	 * always exists.
	 */
	static int get_creation_func_count();

	/**
	 * @brief Gets the name of a given creation function.
	 * @param p_id The creation function ID.
	 * @return The name of the creation function.
	 */
	static const char *get_creation_func_name(int p_id);

	/**
	 * @brief Gets the display function creation ID for the given backend.
	 * @param p_name The name of the backend to search for
	 * @return The display function creation ID, or `0` on failure (which correlates to a headless display).
	 */
	static int get_display_creation_func_id(const String &p_name);

	/**
	 * @brief Creates a window and assigns it as the active window. The width and height of a given window is
	 * calculated from the top-left, with the x axis moving right on the screen and the y axis moving downwards.
	 * @param p_name The title of the window, what appears across the top of the display
	 * @param x The x position of the window
	 * @param y The y position of the window
	 * @param width The number of pixels wide the window will be
	 * @param height The number of pixels tall the window will be
	 * @return The ID of the created window (NOTE: For now, this will remain to be 0 until multiple windows are
	 * supported)
	 */
	virtual uint8_t create_window(const String &p_name,
								  uint16_t x,
								  uint16_t y,
								  uint16_t width,
								  uint16_t height,
								  WindowFlags p_flags = WINDOW_FLAG_DEFAULT) = 0;

	/**
	 * @brief Deletes the currently active window.
	 * @param p_id The current ID of the active window
	 */
	virtual void destroy_window(uint8_t p_id) = 0;

	/**
	 * @brief Toggles whether or not to allow the use of vsync in the application. Limits the number of frames to the
	 * monitor's refresh rate.
	 * @param p_value The value for v-sync: `true` for enabled and `false` for disabled.
	 */
	virtual void set_use_vsync(bool p_value) = 0;

	/**
	 * @brief Gets the size of the given window. For end-user applications, prefer the use of `Window.get_size()` over
	 * this method.
	 * @return The size of the window in pixels, or `(0, 0)` on an invalid window ID.
	 */
	virtual Vector2i get_window_size(uint8_t p_id) const = 0;

	/**
	 * @brief Sets the resize callback function for a given window. The subscribing event should be a library function
	 * rather than a user one as behaviour in the latter case may be undesirable.
	 * @param p_method The `CallableMethod` to call on resizing. Must return `void` and have an `int` as its only
	 * argument.
	 * @param p_id The window ID to bind the resize event to. Must be a valid ID returned by `create_window`.
	 */
	virtual void set_window_resize_callback(const CallableMethod &p_method, uint8_t p_id) = 0;

	virtual void set_input_event_dispatch_callback(const CallableMethod &p_method, uint8_t p_id) = 0;

	/**
	 * @brief Toggles the mouse mode, or in other words changes whether the mouse is drawn over the screen or captured
	 * by the engine. If captured, mouse position is locked to the centre of the screen, but relative movements are
	 * still done in the correct directions.
	 * @param p_mode What mode to use. `true` for captured, `false` for non-captured.
	 */
	virtual void toggle_mouse_mode(bool p_mode) = 0;

	/**
	 * @brief Gets what mouse mode is currently in use.
	 * @return `true` if captured, `false` if not.
	 */
	virtual bool get_mouse_mode() const = 0;

	/**
	 * @brief Reads all window events that have accumulated and processes the desired actions as specified by the
	 * implementation. Ideally, this would be called each frame in a rendered application to allow for synchronisation.
	 */
	virtual void process_events() = 0;

	/**
	 * @brief Swaps the currently rendering and presented screen buffers. Should be called at the end of a frame to
	 * show the present image and allow for the renderer to begin writing the next frame.
	 */
	virtual void swap_buffers() = 0;

	/**
	 * @brief The shutdown event. This cleans up any data left over prior to the destruction call. Most data should aim
	 * to be destroyed here.
	 */
	virtual void finalize() = 0;

	DisplayManager();
	virtual ~DisplayManager();
};

VARIANT_CAST_ENUM(DisplayManager::WindowNotification);
VARIANT_CAST_ENUM(DisplayManager::WindowFlags);
