#version 300 es

precision mediump float;

layout(location = 0) in vec2 vertex;
layout(location = 2) in vec2 uv;

// CanvasInstanceData binds to each of these points which are vec4s to make attrib pointers easier to set up
layout(location = 4) in highp vec4 transform_basis;
layout(location = 5) in highp vec4 instance_data_a;
layout(location = 6) in highp vec4 instance_data_b;
layout(location = 7) in highp vec4 instance_data_c;
layout(location = 8) in highp vec4 instance_data_d;

#define vertex_data instance_data_a.xy
#define rect_offset instance_data_a.zw

#define rect_size instance_data_b.xy
#define instance_data_mask int(instance_data_b.z)
#define unused_data instance_data_b.w

#define texture_subcoords_origin instance_data_c.xy
#define texture_subcoords_size instance_data_c.zw
#define input_colour instance_data_d

#define ITEM_USE_ALPHA_ONLY 1 << 0
#define ITEM_USE_TEXTURE_SUBCOORDS 1 << 2
#define ITEM_FLIP_Y 1 << 3
#define ITEM_USE_RECT 1 << 4

layout(std140) uniform CanvasData { // ubo:1
	mat4 canvas_projection;
};

out vec2 frag_uv;
out vec4 colour_interp;
flat out int instance_data_flags;

void main() {
	if (bool(instance_data_mask & ITEM_USE_TEXTURE_SUBCOORDS)) {
		// Note: this only works in indexed rendering. Non-indexed acts differently and should be treated as such.
		int vert_id = gl_VertexID;
		vec2 source_vertex;
		if (vert_id == 0) {
			source_vertex = vec2(0.0, 0.0);
		} else if (vert_id == 1) {
			source_vertex = vec2(1.0, 1.0);
		} else if (vert_id == 2) {
			source_vertex = vec2(0.0, 1.0);
		} else if (vert_id == 3) {
			source_vertex = vec2(1.0, 0.0);
		}

		frag_uv = texture_subcoords_origin + abs(texture_subcoords_size) * source_vertex.xy;
	} else {
		frag_uv = uv;

		if (bool(instance_data_mask & ITEM_FLIP_Y)) {
			if (uv.y == 1.0) {
				frag_uv.y = 0.0;
			}
			if (uv.y == 0.0) {
				frag_uv.y = 1.0;
			}
		}
	}
	instance_data_flags = instance_data_mask;
	vec2 input_vertex;
	if (bool(instance_data_mask & ITEM_USE_RECT)) {
		input_vertex = rect_offset;
	} else {
		input_vertex = vertex_data;
	}
	mat4 model_matrix = mat4(vec4(transform_basis.xy, 0.0, 0.0),
							 vec4(transform_basis.zw, 0.0, 0.0),
							 vec4(0.0, 0.0, 1.0, 0.0),
							 vec4(input_vertex, 0.0, 1.0));

	colour_interp = input_colour;

	vec2 vert = rect_size * vertex;
	vert = (model_matrix * vec4(vert, 0.0, 1.0)).xy;
	gl_Position = canvas_projection * vec4(vert, 0.0, 1.0);
}