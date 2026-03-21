#pragma once

#include "core/data/vector.h"
#include "core/string/vstring.h"
#include "core/typedefs.h"

struct Uniform {
	String name;
	int loc;
};

struct UBO {
	String name;
	int index;
};

struct ShaderData {
	u32 id;
	u32 vert_id;
	u32 frag_id;

	Vector<Uniform> uniforms;
	Vector<UBO> ubos;

	void shader_set_active();
	void shader_find_uniforms_from_source(const String &p_source);
	Error _setup(const String &p_source_path); // WARNING: DO NOT CALL!!!! IS CALLED BY THE OVERRIDE
	virtual Error _init() = 0;

	ShaderData() {
		id = 0;
		vert_id = 0;
		frag_id = 0;
	}
};

struct SceneShader : ShaderData {
	virtual Error _init() override;
};

struct CanvasShader : ShaderData {
	virtual Error _init() override;
};

struct CopyShader : ShaderData {
	virtual Error _init() override;
};