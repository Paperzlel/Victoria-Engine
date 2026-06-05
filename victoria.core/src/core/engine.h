#pragma once

#include "core/object/object.h"
#include "core/typedefs.h"

class VCORE_API Engine : public Object {
	VREGISTER_CLASS(Engine, Object);

	uint64_t engine_frames = 0;

	static Engine *singleton;

public:
	/**
	 * @brief Gets the engine singleton.
	 */
	static Engine *get_singleton();

	/**
	 * @brief Increments the number of frames the engine has has pass in its lifetime. Implementations of `core` should
	 * always update this number at the end of a render cycle (if applicable). This is used in some places within
	 * `core`, so treat it with care. Not exposed to end-user scripting for obvious reasons.
	 */
	void increment_engine_frames();

	/**
	 * @brief Gets the number of frames that have passed in the engine lifetime. This number may be used in any context
	 * but do note that the use of it may not be valid if the implementation does not properly increment frame counts.
	 * @return The number of frames created in the engine lifetime.
	 */
	uint64_t get_engine_frame_count();

	Engine();
	~Engine();
};
