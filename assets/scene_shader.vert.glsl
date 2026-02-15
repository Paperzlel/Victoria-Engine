#version 460 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 frag_pos;
out vec3 frag_normal;
out vec2 frag_uv;

layout (std140) uniform SceneData { //ubo:0
   mat4 camera_projection;
   mat4 camera_view;
   float time;

   uint directional_lights_used;
};

uniform mat4 transform;

void main() {
   frag_pos = vec3(transform * vec4(position, 1.0));
   // Expensive to do, but saves a uniform binding
   frag_normal = normalize(mat3(transpose(inverse(transform))) * normal);

   frag_uv = uv;

   gl_Position = camera_projection * camera_view * transform * vec4(position, 1.0);
}