#pragma once

#include <core/data/vector.h>
#include <core/string/vstring.h>

class GLShader {
	uint32_t id;

	String _add_shader_information(const char *p_shader);

public:
	struct Uniform {
		const char *name;
		int loc;
	};

	struct UBO {
		const char *name;
		int index;
	};

	// NOTE: A FixedVector of some description would be better to avoid CoW semantics

	Vector<Uniform> uniforms;
	Vector<UBO> ubos;

	void shader_set_active();
	void shader_delete();

	virtual void _init() = 0;
	void _setup(const char *p_vertex_source,
				const char *p_fragment_source,
				const char **p_uniforms,
				int p_uniform_count,
				UBO *p_ubos,
				int p_ubo_count);
};
