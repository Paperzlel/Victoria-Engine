#include "rendering/opengl/shaders_gl.h"
#include "rendering/opengl/utilities.h"

#include <core/io/filesystem.h>
#include <core/os/os.h>
#include <glad/gl.h>

using namespace GL;

String GLShader::_add_shader_information(const char *p_shader) {
	if (!p_shader) {
		return nullptr;
	}

	String ret;
	if (OS::get_singleton()->is_gles_over_gl()) {
		ret += "#version 300 es\n\n";
	} else {
		ret += "#version 330 core\n\n";
	}

	ret += "precision mediump float;\n\n";
	ret += p_shader;
	return ret;
}

void GLShader::shader_set_active() {
	glUseProgram(id);
}

void GLShader::shader_delete() {
	glDeleteProgram(id);
	id = 0;
}

void GLShader::_setup(const char *p_vertex_source,
					  const char *p_fragment_source,
					  const char **p_uniforms,
					  int p_uniform_count,
					  UBO *p_ubos,
					  int p_ubo_count) {
	Error err = OK;
	GLuint vert = 0;
	GLuint frag = 0;
	{
		// Cannot get pointer to an rvalue, so save as a const pointer
		String vertex_source = _add_shader_information(p_vertex_source);
		const char *cstr = vertex_source.ptr();

		vert = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert, 1, &cstr, nullptr);
		glCompileShader(vert);
		Error err = Utilities::get_singleton()->check_pipeline_errors(vert, Utilities::STATUS_COMPILE);
		if (err != OK) {
			ERR_FAIL_MSG("Failed to compile vertex shader.");
		}
	}

	{
		// Cannot get pointer to an rvalue, so save as a const pointer
		String fragment_source = _add_shader_information(p_fragment_source);
		const char *cstr = fragment_source.ptr();

		frag = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag, 1, &cstr, nullptr);
		glCompileShader(frag);
		Error err = Utilities::get_singleton()->check_pipeline_errors(frag, Utilities::STATUS_COMPILE);
		if (err != OK) {
			ERR_FAIL_MSG("Failed to compile fragment shader.");
		}
	}

	id = glCreateProgram();
	glAttachShader(id, vert);
	glAttachShader(id, frag);
	glLinkProgram(id);
	err = Utilities::get_singleton()->check_pipeline_errors(id, Utilities::STATUS_LINK);
	if (err != OK) {
		ERR_FAIL_MSG("Failed to link shader program.");
	}

	glDeleteShader(vert);
	glDeleteShader(frag);

	glUseProgram(id);
	// Setup uniforms
	for (int i = 0; i < p_uniform_count; i++) {
		Uniform u;
		u.name = p_uniforms[i];
		u.loc = glGetUniformLocation(id, p_uniforms[i]);
		// Don't bother with uniform check since they should ALWAYS be in the shader at compile time.
		uniforms.append(u);
	}

	// Setup UBOs
	for (int i = 0; i < p_ubo_count; i++) {
		int loc = glGetUniformBlockIndex(id, p_ubos[i].name);
		if (loc >= 0) {
			glUniformBlockBinding(id, loc, p_ubos[i].index);
		}
		ubos.append(p_ubos[i]);
	}

	glUseProgram(0);
}
