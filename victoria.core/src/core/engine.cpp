#include "core/engine.h"

Engine *Engine::singleton = nullptr;

Engine *Engine::get_singleton() {
	return singleton;
}

void Engine::increment_engine_frames() {
	engine_frames++;
}

uint64_t Engine::get_engine_frame_count() {
	return engine_frames;
}

Engine::Engine() {
	singleton = this;
}

Engine::~Engine() {
	engine_frames = 0;
	singleton = nullptr;
}
