#include "rendering/opengl/utilities.h"

#include "rendering/opengl/rendering_server_gl.h"

#include <core/os/os.h>
#include <core/string/print_string.h>
#include <core/string/vstring.h>

using namespace GL;

/* clang-format off */
static const char *buffer_names[14] = {
    "GL_ARRAY_BUFFER",
    "GL_ATOMIC_COUNTER_BUFFER",
    "GL_COPY_READ_BUFFER",
    "GL_COPY_WRITE_BUFFER",
    "GL_DISPATCH_INDIRECT_BUFFER",
    "GL_DRAW_INDIRECT_BUFFER",
    "GL_ELEMENT_ARRAY_BUFFER",
    "GL_PIXEL_PACK_BUFFER",
    "GL_PIXEL_UNPACK_BUFFER",
    "GL_SHADER_STORAGE_BUFFER",
    "GL_TEXTURE_BUFFER",
    "GL_TRANSFORM_FEEDBACK_BUFFER",
    "GL_UNIFORM_BUFFER"
};
/* clang-format on */

const char *get_buffer_name(const GLenum p_type) {
	switch (p_type) {
		case GL_ARRAY_BUFFER:
			return buffer_names[0];
		case GL_ATOMIC_COUNTER_BUFFER:
			return buffer_names[1];
		case GL_COPY_READ_BUFFER:
			return buffer_names[2];
		case GL_COPY_WRITE_BUFFER:
			return buffer_names[3];
		case GL_DISPATCH_INDIRECT_BUFFER:
			return buffer_names[4];
		case GL_DRAW_INDIRECT_BUFFER:
			return buffer_names[5];
		case GL_ELEMENT_ARRAY_BUFFER:
			return buffer_names[6];
		case GL_PIXEL_PACK_BUFFER:
			return buffer_names[7];
		case GL_PIXEL_UNPACK_BUFFER:
			return buffer_names[8];
		case GL_SHADER_STORAGE_BUFFER:
			return buffer_names[9];
		case GL_TEXTURE_BUFFER:
			return buffer_names[10];
		case GL_TRANSFORM_FEEDBACK_BUFFER:
			return buffer_names[11];
		case GL_UNIFORM_BUFFER:
			return buffer_names[12];
	}
	return "";
}

Utilities *Utilities::singleton = nullptr;

Utilities *Utilities::get_singleton() {
	return singleton;
}

bool Utilities::Buffer::operator==(const Buffer &p_other) const {
	return type == p_other.type;
}

void Utilities::report_buffer_allocations() {
	if (allocations.is_empty()) {
		OS::get_singleton()->print("OpenGL: No current buffer allocations made");
		return;
	}

	OS::get_singleton()->print("OpenGL: Current buffer allocations:");
	for (int i = 0; i < allocations.size(); i++) {
		const char *buffer_name = get_buffer_name(allocations[i].type);
		String str = vformat("%s: %llu bytes", buffer_name, allocations[i].size);
		OS::get_singleton()->print(str.get_data());
	}
}

void Utilities::allocate_buffer(const GLenum p_type, uint64_t p_size, const void *p_ptr, const GLenum p_draw_type) {
	glBufferData(p_type, p_size, p_ptr, p_draw_type);
	Buffer buf;
	buf.type = p_type;

	// Create the buffer if it doesn't exist already
	if (allocations.find(buf) == -1) {
		buf.size = p_size;
		allocations.push_back(buf);
		return;
	}

	for (Buffer &b : allocations) {
		if (b.type != p_type) {
			continue;
		} else {
			b.size += p_size;
			return;
		}
	}
}

void Utilities::allocate_buffer(const GLenum p_type, uint64_t p_size, const GLenum p_draw_type) {
	allocate_buffer(p_type, p_size, nullptr, p_draw_type);
}

void Utilities::free_buffer(const GLenum p_type, uint64_t p_size, const GLuint *p_buffer) {
	glDeleteBuffers(1, p_buffer);

	for (Buffer &b : allocations) {
		if (b.type == p_type) {
			b.size -= p_size;
			if (b.size <= 0) {
				int idx = allocations.find(b);
				allocations.remove_at(idx);
				continue;
			}
		}
	}
}

Error Utilities::check_pipeline_errors(uint32_t p_what, GLStatusType p_compile_type) {
	int success = 0;
	char info_log[1024];
	switch (p_compile_type) {
		case STATUS_COMPILE: {
			glGetShaderiv(p_what, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(p_what, 1024, nullptr, info_log);
				String fmt = vformat("OpenGL: Shader compilation error.\n%s", info_log);
				OS::get_singleton()->print_error(__FILE__, FUNCTION_STR, __LINE__, fmt.get_data(), "", ERROR_SHADER);
				return ERR_CANT_CREATE;
			}
		} break;
		case STATUS_LINK: {
			glGetProgramiv(p_what, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(p_what, 1024, nullptr, info_log);
				OS::get_singleton()->printerr("OpenGL shader linking error.\n%s", info_log);
				return ERR_CANT_CREATE;
			}
		} break;
		default: {
			OS::get_singleton()->print_error(
				__FILE__,
				FUNCTION_STR,
				__LINE__,
				vformat("OpenGL: Shader pipeline error handler was passed an invalid error of %i.", p_compile_type),
				"",
				ERROR_SHADER);
			return ERR_INVALID_PARAMETER;
		} break;
	}
	return OK;
}

/* clang-format off */
const char *source_location_strings[] = {
    "The source API",
    "The GLSL compiler",
    "The windowing system",
    "A third party",
    "The application",
    "An unknown source"
};

const char *severity_strings[] = {
    "fatal",
    "serious",
    "warning",
    "notification"
};

const char *error_type_strings[] = {
    "",
    "deprecated behaviour",
    "undefined behaviour",
    "portability",
    "performance",
    "miscellaneous",
    "marker"
};
/* clang-format on */

void Utilities::debug_message_callback(GLenum p_source,
									   GLenum p_type,
									   GLenum p_id,
									   GLenum p_severity,
									   GLsizei p_length,
									   const GLchar *p_message,
									   const void *p_user_param) {
	if (p_severity == GL_DEBUG_SEVERITY_NOTIFICATION && !is_printing_verbose()) {
		return; // Ignore notifications as they're not very important for us to handle
	}

	uint8_t source_id = p_source - GL_DEBUG_SOURCE_API;
	uint8_t severity = (p_severity == GL_DEBUG_SEVERITY_NOTIFICATION) ? 3 : p_severity - GL_DEBUG_SEVERITY_HIGH;
	uint8_t error_type = (p_type == GL_DEBUG_TYPE_MARKER) ? 6 : p_type - GL_DEBUG_TYPE_ERROR;

	// Is fatal/serious --> error
	// Is warning/notification --> no error

	String error_string;

	if (severity > 1) {
		// Non-error
		error_string = vformat("OpenGL: %s gave a %s %s: \n%s",
							   source_location_strings[source_id],
							   error_type_strings[error_type],
							   severity_strings[severity],
							   p_message);
	} else {
		// Error
		error_string = vformat("OpenGL: %s gave a %s %s error: \n%s",
							   source_location_strings[source_id],
							   severity_strings[severity],
							   error_type_strings[error_type],
							   p_message);
	}

	if (p_severity <= GL_DEBUG_SEVERITY_MEDIUM) {
		OS::get_singleton()->printerr(error_string.get_data());
	} else if (is_printing_verbose()) {
		OS::get_singleton()->print_error(__FILE__, FUNCTION_STR, __LINE__, error_string, "", ERROR_DEBUG);
	}
}

RS::InstanceType Utilities::get_base_type(RID p_base) {
	RenderingServerGL *rs = static_cast<RenderingServerGL *>(RS::get_singleton());
	if (rs->owns_mesh(p_base)) {
		return RS::INSTANCE_TYPE_MESH;
	} else if (rs->owns_light(p_base)) {
		return RS::INSTANCE_TYPE_LIGHT;
	} else {
		return RS::INSTANCE_TYPE_NONE;
	}
}

Utilities::Utilities() {
	singleton = this;
}

Utilities::~Utilities() {
	singleton = nullptr;
}