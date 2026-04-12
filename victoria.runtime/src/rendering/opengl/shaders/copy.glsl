#[vertex]

layout(location = 0) in vec2 vertex;

out vec2 frag_uv;

uniform vec4 offset_size;

void main() {
	// NDC states -1, -1 to 1, 1, we want range (0, 1)
	frag_uv = vertex * 0.5 + 0.5;
	gl_Position = vec4(vertex, 0.0, 1.0);
	gl_Position.xy = (offset_size.xy + frag_uv.xy * offset_size.zw) * 2.0 - 1.0;
}

#[fragment]

uniform sampler2D texture_sampler;

in vec2 frag_uv;
out vec4 frag_colour;

void main() {
	frag_colour = texture(texture_sampler, frag_uv);
}
