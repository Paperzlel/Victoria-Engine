#include "drivers/opengl/shaders_gl.h"

#include "core/io/filesystem.h"
#include "drivers/opengl/utilities.h"

#include <glad/gl.h>

using namespace GL;

void ShaderData::shader_set_active() {
	glUseProgram(id);
}

Error ShaderData::_setup(const String &p_source_path) {
	Ref<FileSystem> fs;

	Error err;

	// Vertex shader stage
	{
		String n_vert_path = p_source_path + ".vert.glsl";
		fs = FileSystem::open(n_vert_path, FileSystem::FILE_ACCESS_READ);
		String vertex_contents = fs->get_contents_as_string();
		fs->close();
		// Manual cast as otherwise the data is lost
		const char *vertex_cstr = vertex_contents.get_data();

		vert_id = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert_id, 1, &vertex_cstr, nullptr);
		glCompileShader(vert_id);

		err = Utilities::get_singleton()->check_pipeline_errors(vert_id, Utilities::STATUS_COMPILE);
		if (err != OK) {
			ERR_FAIL_MSG_R("Failed to compile vertex shader.", err);
		}

		shader_find_uniforms_from_source(vertex_contents);
	}

	// Fragment shader stage
	{
		String n_vert_path = p_source_path + ".frag.glsl";
		fs = FileSystem::open(n_vert_path, FileSystem::FILE_ACCESS_READ);
		String fragment_contents = fs->get_contents_as_string();
		fs->close();
		const char *fragment_cstr = fragment_contents.get_data();

		frag_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag_id, 1, &fragment_cstr, nullptr);
		glCompileShader(frag_id);

		err = Utilities::get_singleton()->check_pipeline_errors(frag_id, Utilities::STATUS_COMPILE);
		if (err != OK) {
			ERR_FAIL_MSG_R(vformat("Failed to compile fragment shader \'%s\'", n_vert_path.get_data()), err);
		}

		shader_find_uniforms_from_source(fragment_contents);
	}

	// Shader linking stage
	{
		id = glCreateProgram();
		glAttachShader(id, vert_id);
		glAttachShader(id, frag_id);
		glLinkProgram(id);

		err = Utilities::get_singleton()->check_pipeline_errors(id, Utilities::STATUS_LINK);
		if (err != OK) {
			return err;
		}

		glDeleteShader(vert_id);
		glDeleteShader(frag_id);
	}

	glUseProgram(id);
	// Get uniform locations and bind them
	for (int i = 0; i < uniforms.size(); i++) {
		Uniform &u = uniforms[i];
		u.loc = glGetUniformLocation(id, u.name);
		if (u.loc == -1) {
			u.loc = 0;
			// This should NEVER happen, since we get the uniforms from the code itself
			ERR_WARN(vformat("The uniform \"%s\" does not exist within the shader.", u.name.get_data()));
			continue;
		}
	}

	for (int i = 0; i < ubos.size(); i++) {
		UBO &ub = ubos[i];
		int loc = glGetUniformBlockIndex(id, ub.name.get_data());
		if (loc >= 0) {
			glUniformBlockBinding(id, loc, ub.index);
		}
	}

	glUseProgram(0);
	return OK;
}

void ShaderData::shader_find_uniforms_from_source(const String &p_source) {
	Vector<String> lines = p_source.split("\n");
	for (String &line : lines) {
		if (line.begins_with("//")) {
			continue;
		}

		int comment_pos = line.find("//");
		if (comment_pos != -1) {
			if (!line.contains("ubo")) {
				line = line.left(comment_pos);
			}
		}

		// Check for CR (since splitting by CRLF doesn't work on every file system)
		if (line.ends_with("\r")) {
			line = line.left(-1);
		}

		// Could be a uniform
		if (line.contains("uniform")) {
			Vector<String> indicators = line.split(" ");
			// is a UBO
			if (line.contains("layout")) {
				UBO ub;
				ub.name = indicators[2]; // Should be #3 in the array, could change if someone uses manual bindings
				// Binding a UBO to a layout goes something like "{ // ubo:2"
				ub.index = indicators[5].right(1).to_int();
				ubos.push_back(ub);
			} else {
				Uniform un;
				un.name = indicators[2].left(-1);
				un.loc = 0;
				uniforms.push_back(un);
			}
		}
	}
}

Error SceneShader::_init() {
	return _setup("assets/scene_shader");
}

Error CanvasShader::_init() {
	return _setup("assets/canvas_shader");
}

Error CopyShader::_init() {
	return _setup("assets/copy");
}