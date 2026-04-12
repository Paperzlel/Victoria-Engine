#pragma once

#include "rendering/rendering_server.h"

#include <core/data/rid.h>
#include <core/data/vector.h>
#include <core/math/mat4.h>
#include <glad/gl.h>

namespace GL {

	class Utilities {
	private:
		struct Buffer {
			GLenum type;
			uint64_t size;

			bool operator==(const Buffer &p_other) const; // For compat with Vector<T>::find()
		};

		Vector<Buffer> allocations;

		static Utilities *singleton;

	public:
		enum GLStatusType {
			STATUS_COMPILE,
			STATUS_LINK,
		};

		static Utilities *get_singleton();

		static FORCE_INLINE void store_mat4(const Mat4 &p_mat, float *p_array) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					p_array[i * 4 + j] = p_mat[i * 4 + j];
				}
			}
		}

		static FORCE_INLINE void store_vec3(const Vector3 &p_vec, float *p_array) {
			for (int i = 0; i < 3; i++) {
				p_array[i] = p_vec[i];
			}
		}

		static FORCE_INLINE void store_vec4(const Vector4 &p_vec, float *p_array) {
			for (int i = 0; i < 4; i++) {
				p_array[i] = p_vec[i];
			}
		}

		void report_buffer_allocations();

		void allocate_buffer(const GLenum p_type, uint64_t p_size, const void *p_ptr, const GLenum p_draw_type);
		void allocate_buffer(const GLenum p_type, uint64_t p_size, const GLenum p_draw_type);

		void free_buffer(const GLenum p_type, uint64_t p_size, const GLuint *p_buffer);

		Error check_pipeline_errors(uint32_t p_what, GLStatusType p_compile_type);
		static void debug_message_callback(GLenum p_source,
										   GLenum p_type,
										   GLenum p_id,
										   GLenum p_severity,
										   GLsizei p_length,
										   const GLchar *p_message,
										   const void *p_user_param);

		RS::InstanceType get_base_type(RID p_base);

		Utilities();
		~Utilities();
	};

}; // namespace GL