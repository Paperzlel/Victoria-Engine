#version 300 es

precision mediump float;

#define MAX_LIGHT_COUNT 32

in vec3 frag_normal;
in vec3 frag_pos;
in vec2 frag_uv;

out highp vec4 frag_colour;

layout(std140) uniform SceneData { // ubo:0
	mat4 camera_projection;
	mat4 camera_view;
	highp float time;

	uint directional_lights_used;
};

layout(std140) uniform MaterialData { // ubo:2
	vec4 diffuse;
	vec3 ambient;
	vec3 specular;
	float shininess;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float range;
};

layout(std140) uniform PointLightData { // ubo:3
	PointLight point_lights[MAX_LIGHT_COUNT];
};

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

layout(std140) uniform DirectionalLightData { // ubo:4
	DirectionalLight directional_lights[MAX_LIGHT_COUNT];
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float inner_cut_off;

	vec3 ambient;
	float outer_cut_off;
	vec3 diffuse;
	vec3 specular;

	float range;
};

layout(std140) uniform SpotLightData { // ubo:5
	SpotLight spot_lights[MAX_LIGHT_COUNT];
};

uniform uint point_lights_used;
uniform uint spot_lights_used;
uniform vec3 view_pos;
uniform sampler2D texture_sampler;

vec3 calculate_point_light(PointLight in_light, vec3 fragment, vec3 view_direction) {
	vec3 light_direction = normalize(in_light.position - fragment);

	// Diffuse lighting
	float diff = max(dot(frag_normal, light_direction), 0.0);

	// Specular lighting
	vec3 reflect_direction = reflect(-light_direction, frag_normal);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), shininess);
	// Lambert term
	// (https://stackoverflow.com/questions/20008089/specular-lighting-appears-on-both-eye-facing-and-rear-sides-of-object)
	// Fixes specular reflection on the opposite side of the cube
	spec *= diff;
	// Attenuation
	float dist = length(in_light.position - frag_pos);
	// Use a equation in the form 1 / (c + lx + qx^2) where c = 1, 0 < l < 1 and 0 < q < 1.
	// https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
	float attenuation = 0.0;
	if (dist < in_light.range) {
		attenuation = 1.0 / (1.0 + 0.08 * dist + 0.025 * (dist * dist));
	}

	vec3 ambient = in_light.ambient * vec3(diffuse.rgb);
	vec3 diffuse2 = in_light.diffuse * diff * vec3(diffuse.rgb);
	vec3 specular = in_light.specular * spec * specular;

	// ambient *= attenuation;
	diffuse2 *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse2 + specular);
}

vec3 calculate_directional_light(DirectionalLight in_light, vec3 view_direction) {
	vec3 light_direction = normalize(mat3(camera_view) * -in_light.direction);

	// Diffuse lighting
	float diff = max(dot(frag_normal, light_direction), 0.0);

	// Specular lighting (see point light for extra notes)
	vec3 reflect_direction = reflect(-light_direction, frag_normal);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), shininess);
	spec *= diff;

	vec3 ambient = in_light.ambient * vec3(diffuse.rgb);
	vec3 diffuse2 = in_light.diffuse * diff * vec3(diffuse.rgb);
	vec3 specular = in_light.specular * spec * specular;

	return (ambient + diffuse2 + specular);
}

vec3 calculate_spot_light(SpotLight in_light, vec3 view_direction) {
	vec3 light_direction = normalize(in_light.position - frag_pos);

	float theta = dot(light_direction, normalize(-in_light.direction));

	// Check if the angle between outer plane and inner plane is less (remember cosines)
	if (theta > in_light.outer_cut_off) {
		// Diffuse lighting
		float diff = max(dot(frag_normal, light_direction), 0.0);

		// Specular lighting
		vec3 reflect_direction = reflect(-light_direction, frag_normal);
		float spec = pow(max(dot(view_direction, reflect_direction), 0.0), shininess);
		spec *= diff;

		vec3 ambient = in_light.ambient * vec3(diffuse.rgb);
		vec3 diffuse2 = in_light.diffuse * diff * vec3(diffuse.rgb);
		vec3 specular = in_light.specular * spec * specular;

		float dist = length(in_light.position - frag_pos);
		float attenuation = 0.0;
		if (dist < in_light.range) {
			attenuation = 1.0 / (1.0 + 0.08 * dist + 0.025 * (dist * dist));
		}

		float epsilon = in_light.inner_cut_off - in_light.outer_cut_off;
		float intensity = clamp((theta - in_light.outer_cut_off) / epsilon, 0.0, 1.0);

		diffuse2 *= intensity;
		diffuse2 *= intensity;

		// ambient *= attenuation;
		diffuse2 *= attenuation;
		specular *= attenuation;

		return (ambient + diffuse2 + specular);
	} else {
		return ambient;
	}
}

void main() {
	// Calculate the view direction
	vec3 view_dir = normalize(view_pos - frag_pos);

	vec3 result = vec3(0.0);
	// Directional lights
	if (directional_lights_used > ((uint)0)) {) {
			for (uint i = 0; i < directional_lights_used; i++) {
				result += calculate_directional_light(directional_lights[i], view_dir);
			}
		}
	}

	// Point lights
	if (point_lights_used > ((uint)0)) {
		for (uint i = 0; i < point_lights_used; i++) {
			result += calculate_point_light(point_lights[i], frag_pos, view_dir);
		}
	}

	if (spot_lights_used > ((uint)0)) {
		for (uint i = 0; i < spot_lights_used; i++) {
			result += calculate_spot_light(spot_lights[i], view_dir);
		}
	}

	if (directional_lights_used == ((uint)0) && point_lights_used == ((uint)0) && spot_lights_used == ((uint)0)) {
		result = ambient;
	}

	frag_colour = texture(texture_sampler, frag_uv) * vec4(result, diffuse.a);
}