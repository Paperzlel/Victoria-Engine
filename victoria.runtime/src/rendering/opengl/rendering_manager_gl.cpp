#include "rendering/opengl/rendering_manager_gl.h"

#include <core/io/input.h>
#include <core/math/mat4.h>
#include <core/math/math_funcs.h>
#include <core/math/transform_3d.h>
#include <core/os/os.h>
#include <core/string/print_string.h>

#include <glad/egl.h>
#include <glad/gl.h>
#include <stb/stb_image.h>

bool polygon_line_mode = false;

using namespace GL;

static void *_egl_load_function_wrapper(const char *data) {
	return (void *)eglGetProcAddress(data);
}

bool RenderingManagerGL::use_gles_over_gl = false;

typedef void(GLAPIENTRY *PFNDEBUGMESSAGECALLBACK)(GLDEBUGPROC, const void *);

/**
 * @brief Converts a Transform2D to a Transform3D (shouldn't need to per se, but we do regardless).
 */
Transform3D transform2d_to_3d(const Transform2D &p_base) {
	Transform3D ret;
	ret.basis.data[0] = p_base.basis[0];
	ret.basis.data[1] = p_base.basis[1];
	ret.basis.data[4] = p_base.basis[2];
	ret.basis.data[5] = p_base.basis[3];

	ret.position = Vector3(p_base.position.x, p_base.position.y, 0);
	return ret;
}

/**
 * @brief Converts a `Mat4` into an array of floats to upload to the GPU. Since double-precision isn't supported for
 * positions in GLSL, we have to manually convert the matrix from doubles into floats, which is done by this function.
 * @param p_location The location of the given shader in our current GL context
 * @param p_matrix The matrix to set the uniform location to
 */
void RenderingManagerGL::set_uniform_mat4(int p_location, const Mat4 &p_matrix) {
	float matrix[16];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix[i * 4 + j] = p_matrix[i * 4 + j];
		}
	}

	glUniformMatrix4fv(p_location, 1, GL_TRUE, matrix);
}

void RenderingManagerGL::set_uniform_vec3(int p_location, const Vector3 &p_vector) {
	float vec[3];
	for (int i = 0; i < 3; i++) {
		vec[i] = p_vector[i];
	}

	glUniform3fv(p_location, 1, &vec[0]);
}

void RenderingManagerGL::set_uniform_vec4(int p_location, const Vector4 &p_vector) {
	float vec[4];
	for (int i = 0; i < 4; i++) {
		vec[i] = p_vector[i];
	}

	glUniform4fv(p_location, 1, &vec[0]);
}

void RenderingManagerGL::transform2d_store_mat2x3(const Transform2D &p_transform, float *mat2x3) {
	mat2x3[0] = p_transform.basis[0];
	mat2x3[1] = p_transform.basis[1];
	mat2x3[2] = p_transform.basis[2];
	mat2x3[3] = p_transform.basis[3];
	mat2x3[4] = p_transform.position[0];
	mat2x3[5] = p_transform.position[1];
}

bool RenderingManagerGL::owns_mesh(RID p_mesh) {
	Mesh *mesh = mesh_owner.get_or_null(p_mesh);
	return mesh != nullptr;
}

bool RenderingManagerGL::owns_light(RID p_light) {
	Light *light = light_owner.get_or_null(p_light);
	return light != nullptr;
}

Error RenderingManagerGL::initialize() {
	set_depth_testing(true);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (polygon_line_mode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	shaders.scene_shader._init();
	shaders.canvas_shader._init();
	shaders.copy_shader._init();

	return OK;
}

void RenderingManagerGL::finalize() {
	Vector<RID> owned_meshes;
	mesh_owner.get_owned_list(&owned_meshes);
	for (RID &p_rid : owned_meshes) {
		mesh_free(p_rid);
	}

	Vector<RID> owned_textures;
	texture_owner.get_owned_list(&owned_textures);
	for (RID &p_rid : owned_textures) {
		texture_free(p_rid);
	}

	shaders.scene_shader.shader_delete();
	shaders.canvas_shader.shader_delete();
	shaders.copy_shader.shader_delete();

	Memory::vfree(scene_data.point_lights);
	utils->free_buffer(GL_UNIFORM_BUFFER, sizeof(SceneData::PointLight) * 32, &scene_data.point_light_buffer);

	Memory::vfree(scene_data.directional_lights);
	utils->free_buffer(GL_UNIFORM_BUFFER,
					   sizeof(SceneData::DirectionalLight) * 32,
					   &scene_data.directional_light_buffer);

	Memory::vfree(scene_data.spot_lights);
	utils->free_buffer(GL_UNIFORM_BUFFER, sizeof(SceneData::SpotLight) * 32, &scene_data.spot_light_buffer);

	utils->free_buffer(GL_UNIFORM_BUFFER, sizeof(SceneData::UBO), &scene_data.ubo_buffer);

	Memory::vfree(canvas_data.canvas_instance_data);
	utils->free_buffer(GL_ARRAY_BUFFER,
					   sizeof(CanvasInstanceData) * MAX_INSTANCE_DATA_COUNT,
					   &canvas_data.canvas_instance_data_buffer);

	utils->free_buffer(GL_UNIFORM_BUFFER, sizeof(CanvasData::UBO), &canvas_data.canvas_buffer);

	Material *defmat = material_owner.get_or_null(default_material);
	utils->free_buffer(GL_UNIFORM_BUFFER, sizeof(MaterialData), &defmat->buffer);
	material_free(default_material);

	utils->free_buffer(GL_ARRAY_BUFFER, sizeof(float) * 16, &canvas_data.rect_vertex_buffer);
	utils->free_buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6, &canvas_data.rect_index_buffer);
	glDeleteVertexArrays(1, &canvas_data.rect_vertex_array);

	glDeleteBuffers(1, &canvas_data.screen_quad);
	glDeleteVertexArrays(1, &canvas_data.screen_quad_array);

	canvas_data.batches.clear();
	active_viewports.clear();
	sorted_active_viewports.clear();

	// Uncomment when checking for memory leaks
	// utils->report_buffer_allocations();
}

/**
 * @brief Sorts the active viewports from most nested to the root window. Since each framebuffer can reference a
 * viewport's texture, the most nested viewport must be drawn first to give the correct texture to the other viewports.
 * @returns The sorted viewports.
 */
Vector<RenderingManagerGL::Viewport *> RenderingManagerGL::_get_sorted_viewports() {
	// Sort from right to left, since render order must be in reverse tree order

	Vector<Viewport *> sorted;
	List<Viewport *> next;

	// Tree only knows parent + child
	// So sort nodes at root first
	for (int i = active_viewports.size() - 1; i >= 0; i--) {
		Viewport *vp = active_viewports[i];
		if (vp->parent.is_valid()) {
			continue;
		}

		next.push_back(vp);
		sorted.insert(vp, 0);
	}

	// Now, sort from linked-list.
	// Need to check for non-root nodes
	while (next.size()) {
		// Remove pending from queue
		Viewport *v = next.front()->get();
		next.pop_front();

		for (int i = active_viewports.size() - 1; i >= 0; i--) {
			Viewport *child = active_viewports[i];
			if (child->parent != v->self) {
				continue;
			}

			if (!next.find(child)) {
				next.push_back(child);
				sorted.insert(child, 0);
			}
		}
	}

	return sorted;
}

void RenderingManagerGL::_render_internal(RenderData *r_data) {
	if (Input::get_singleton()->is_key_just_released(KEY_F3)) {
		utils->report_buffer_allocations();
	}

	// - Renders Viewport 1 first (get scene list --> render, get item list (empty) --> render, render viewport
	// texture)
	// - Then renders parent viewport

	if (sorted_viewports_dirty) {
		sorted_active_viewports = _get_sorted_viewports();
		sorted_viewports_dirty = false;
	}

	// Render each viewport from most to least nested
	double start_time = OS::get_singleton()->get_os_running_time();
	for (int i = 0; i < sorted_active_viewports.size(); i++) {
		Viewport *vp = sorted_active_viewports[i];

		_prepare_viewport(vp);

		// Do some sanity checking for things like cameras and so forth
		if (vp->camera.is_valid()) {
			// Draw 3D scene
			Camera *cam = camera_owner.get_or_null(vp->camera);
			if (cam) {
				_render_scene(r_data, vp, cam);
			}
		}

		Canvas *cv = canvas_owner.get_or_null(vp->canvas);
		if (cv) {
			// Draw 2D canvas
			_render_canvas(r_data, vp, cv);
		}

		// Draw viewport
		// Viewport needs to be rendered only if it's the root window.
		if (vp->viewport_to_window != -1) {
			_render_viewport(r_data, vp);
		}
	}

	double end_time = OS::get_singleton()->get_os_running_time();
	r_data->render_time = end_time - start_time;
}

void RenderingManagerGL::_render_scene(RenderData *r_data, Viewport *p_viewport, Camera *p_camera) {
	// Scene pass
	{
		Transform3D inv_cam_view = p_camera->view.inverse();
		Camera *cam = p_camera;

		shaders.scene_shader.shader_set_active();

		Vector<GeometryData> geom_instances;
		uint32_t point_light_count = 0;
		uint32_t directional_light_count = 0;
		uint32_t spot_light_count = 0;
		Vector<GLShader::Uniform> scene_uniforms = shaders.scene_shader.uniforms;

		Vector<RID> instance_list;
		instance_owner.get_owned_list(&instance_list);

		// Loop over each instance to check its data
		for (const RID &rid : instance_list) {
			Instance *inst = instance_owner.get_or_null(rid);

			switch (inst->type) {
				case INSTANCE_TYPE_NONE: {
					continue;
				} break;
				case INSTANCE_TYPE_MESH: {
					GeometryData data;

					if (owns_mesh(inst->base)) {
						data.base = inst->base;
					} else {
						data.base = RID();
					}

					data.transform = inst->transform;
					geom_instances.push_back(data);
				} break;
				case INSTANCE_TYPE_LIGHT: {
					Light *light = light_owner.get_or_null(inst->base);

					switch (light->type) {
						case TYPE_DIRECTIONAL_LIGHT: {
							directional_light_count++;

							SceneData::DirectionalLight &p =
								scene_data.directional_lights[directional_light_count - 1];
							Utilities::store_vec3(light->ambient, p.ambient);
							Utilities::store_vec3(light->diffuse, p.diffuse);
							Utilities::store_vec3(light->specular, p.specular);

							// Get the inverted view matrix and base all light off of that
							Vector3 dir; /* = (inv_cam_view.basis *
									 Mat4::translation((inst->transform.basis * Mat4::translation(Vector3(0, 0,
									 1))).get_translation())) .get_translation() .normalized(); */
							Vector3 eulers = inst->transform.basis.get_euler_angles();
							dir.x = Math::sin(eulers.y) * Math::cos(eulers.x);
							dir.y = Math::sin(eulers.x);
							dir.z = Math::cos(eulers.y) * Math::cos(eulers.x);
							dir.normalize();
							Utilities::store_vec3(dir, p.direction);

						} break;
						case TYPE_POINT_LIGHT: {
							point_light_count++;

							SceneData::PointLight &p = scene_data.point_lights[point_light_count - 1];
							Utilities::store_vec3(light->ambient, p.ambient);
							Utilities::store_vec3(light->diffuse, p.diffuse);
							Utilities::store_vec3(light->specular, p.specular);
							Utilities::store_vec3(inst->transform.position, p.position);
							p.range = light->range;
						} break;
						case TYPE_SPOT_LIGHT: {
							spot_light_count++;
							SceneData::SpotLight &s = scene_data.spot_lights[spot_light_count - 1];

							Utilities::store_vec3(light->ambient, s.ambient);
							Utilities::store_vec3(light->diffuse, s.diffuse);
							Utilities::store_vec3(light->specular, s.specular);

							Vector3 pos =
								(inv_cam_view.basis * Mat4::translation(inst->transform.position)).get_translation();

							Utilities::store_vec3(pos, s.position);

							Vector3 dir;
							Vector3 eulers = inst->transform.basis.get_euler_angles();
							dir.x = Math::sin(eulers.y) * Math::cos(eulers.x);
							dir.y = Math::sin(eulers.x);
							dir.z = Math::cos(eulers.y) * Math::cos(eulers.x);
							dir.normalize();

							Utilities::store_vec3(dir, s.direction);

							s.range = light->range;
							s.inner_cut_off = light->inner_radius;
							s.outer_cut_off = light->outer_radius;
						} break;
						default:
							break;
					}
				}
				default: {
				} break;
			};
		}

		// Bind light UBOs
		glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_LIGHT_DATA, scene_data.point_light_buffer);
		glBufferSubData(GL_UNIFORM_BUFFER,
						0,
						sizeof(SceneData::PointLight) * point_light_count,
						scene_data.point_lights);
		scene_data.point_light_count = point_light_count;

		glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_DIRECTIONAL_LIGHT_DATA, scene_data.directional_light_buffer);
		glBufferSubData(GL_UNIFORM_BUFFER,
						0,
						sizeof(SceneData::DirectionalLight) * directional_light_count,
						scene_data.directional_lights);
		scene_data.ubo.directional_light_count = directional_light_count;

		glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_SPOT_LIGHT_DATA, scene_data.spot_light_buffer);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneData::SpotLight) * spot_light_count, scene_data.spot_lights);
		scene_data.spot_light_count = spot_light_count;

		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Bind SceneData UBO
		{
			Utilities::store_mat4(cam->projection.transposed(), scene_data.ubo.projection);
			Utilities::store_mat4(cam->view.get_model().transposed(), scene_data.ubo.view);
			scene_data.ubo.time = OS::get_singleton()->get_os_running_time();

			if (scene_data.ubo_buffer == 0) {
				glGenBuffers(1, &scene_data.ubo_buffer);
				glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_SCENE_DATA, scene_data.ubo_buffer);
				utils->allocate_buffer(GL_UNIFORM_BUFFER, sizeof(SceneData::UBO), &scene_data.ubo, GL_STREAM_DRAW);
			} else {
				glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_SCENE_DATA, scene_data.ubo_buffer);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SceneData::UBO), &scene_data.ubo);
			}
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		// Bind item-independent uniforms
		for (const GLShader::Uniform &u : scene_uniforms) {
			if (vstring_compare(u.name, "view_pos")) {
				// Get position data (since the camera is already inverted position-wise, invert it again)
				Vector3 pos = cam->view.position.inverse();
				set_uniform_vec3(u.loc, pos);
			}
			if (vstring_compare(u.name, "point_lights_used")) {
				glUniform1ui(u.loc, scene_data.point_light_count);
			}
			if (vstring_compare(u.name, "spot_lights_used")) {
				glUniform1ui(u.loc, scene_data.spot_light_count);
			}
		}

		// Actually render the objects
		for (GeometryData &inst : geom_instances) {
			Mesh *mesh = mesh_owner.get_or_null(inst.base);
			if (!mesh) {
				ERR_WARN("Mesh instance could not be found due to an invalid ID and could not be drawn.");
				continue;
			}

			for (const GLShader::Uniform &u : scene_uniforms) {
				if (vstring_compare(u.name, "transform")) {
					set_uniform_mat4(u.loc, inst.transform.get_model());
					break;
				}
			}

			if (mesh->material.is_valid()) {
				// Using custom material
				Material *mat = material_owner.get_or_null(mesh->material);

				if (mat->buffer == 0) {
					glGenBuffers(1, &mat->buffer);
					glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_MATERIAL_DATA, mat->buffer);
					utils->allocate_buffer(GL_UNIFORM_BUFFER, sizeof(MaterialData), mat->data, GL_DYNAMIC_DRAW);
				} else {
					glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_MATERIAL_DATA, mat->buffer);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialData), mat->data);
				}
				glBindBuffer(GL_UNIFORM_BUFFER, 0);

				// Setup textures
				Texture *t = texture_owner.get_or_null(mat->texture);
				if (t) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, t->texture_buffer);
				} else {
					// Load default texture
					Texture *t = texture_owner.get_or_null(default_texture);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, t->texture_buffer);
				}
			} else {
				// Using default material
				Material *defmat = material_owner.get_or_null(default_material);

				glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_MATERIAL_DATA, defmat->buffer);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialData), defmat->data);

				Texture *deft = texture_owner.get_or_null(defmat->texture);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, deft->texture_buffer);
			}

			glBindVertexArray(mesh->vertex_array);
			if (mesh->index_count) {
				glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, nullptr);
			} else {
				glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
			}
			glBindVertexArray(0);

			r_data->draw_calls++;
			r_data->primitive_count += mesh->vertex_count / 3;
		}
	}
}

void RenderingManagerGL::_render_canvas(RenderData *r_data, Viewport *p_viewport, Canvas *p_canvas) {
	// No children to draw, leave
	if (p_canvas->child_items.is_empty()) {
		return;
	}

	// Canvas data UBO pass
	{
		Mat4 ortho = Mat4::orthographic(0, p_viewport->size.y, 0, p_viewport->size.x, 100, -100);
		Utilities::store_mat4(ortho, canvas_data.ubo.projection);

		if (canvas_data.canvas_buffer == 0) {
			glGenBuffers(1, &canvas_data.canvas_buffer);
			glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_CANVAS_DATA, canvas_data.canvas_buffer);
			utils->allocate_buffer(GL_UNIFORM_BUFFER, sizeof(CanvasData::UBO), &canvas_data.ubo, GL_STREAM_DRAW);
		} else {
			glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_CANVAS_DATA, canvas_data.canvas_buffer);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CanvasData::UBO), &canvas_data.ubo);
		}
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	// Canvas shader pass
	{
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		shaders.canvas_shader.shader_set_active();

		// Create a new batch
		_new_canvas_batch();

		uint32_t index = 0;

		// Gather the item data
		for (const Item *i : p_canvas->child_items) {
			Item::Base *b = i->base;
			if (!b || b->type == Item::TYPE_NONE) {
				// Ignore unallocated or unused base types.
				// TODO: Rect information could be stored off in the root item instead of allocating a base.
				continue;
			}

			transform2d_store_mat2x3(i->transform, canvas_data.canvas_instance_data[index].model);

			if (b->material != canvas_data.batches[canvas_data.current_batch].material ||
				i->ysort != canvas_data.batches[canvas_data.current_batch].ysort) {
				_new_canvas_batch();
				canvas_data.batches[canvas_data.current_batch].material = b->material;
				canvas_data.batches[canvas_data.current_batch].ysort = i->ysort;
			}

			for (int j = 0; j < 4; j++) {
				canvas_data.canvas_instance_data[index].rect[j] = 0;
				canvas_data.canvas_instance_data[index].subcoords[j] = 0;
				canvas_data.canvas_instance_data[index].colour[j] = i->colour[j];
			}

			// Initialize flags per-item
			canvas_data.canvas_instance_data[index].masks[0] = i->flags;
			canvas_data.canvas_instance_data[index].masks[1] = 0; // Unused

			switch (b->type) {
				case Item::TYPE_NONE: {
					ERR_WARN("Items should not have a type of none.");
				} break;
				case Item::TYPE_RECT: {
					Item::Rect *r = static_cast<Item::Rect *>(b);

					if (canvas_data.batches[canvas_data.current_batch].type == Item::TYPE_NONE) {
						canvas_data.batches[canvas_data.current_batch].type = Item::TYPE_RECT;
					}

					if (r->texture != canvas_data.batches[canvas_data.current_batch].texture ||
						canvas_data.batches[canvas_data.current_batch].type != Item::TYPE_RECT) {
						_new_canvas_batch();
						canvas_data.batches[canvas_data.current_batch].texture = r->texture;
						canvas_data.batches[canvas_data.current_batch].type = Item::TYPE_RECT;
					} else {
						canvas_data.batches[canvas_data.current_batch].texture = r->texture;
					}

					canvas_data.canvas_instance_data[index].rect[0] = r->rect_offset.x;
					canvas_data.canvas_instance_data[index].rect[1] = r->rect_offset.y;
					canvas_data.canvas_instance_data[index].rect[2] = r->rect_size.x;
					canvas_data.canvas_instance_data[index].rect[3] = r->rect_size.y;

					if (i->flags & ITEM_FLAG_USE_TEXTURE_SUBCOORDS) {
						canvas_data.canvas_instance_data[index].subcoords[0] = r->uv_offset.x;
						canvas_data.canvas_instance_data[index].subcoords[1] = r->uv_offset.y;
						canvas_data.canvas_instance_data[index].subcoords[2] = r->uv_size.x;
						canvas_data.canvas_instance_data[index].subcoords[3] = r->uv_size.y;
					}

					canvas_data.batches[canvas_data.current_batch].instance_count++;
					index++;
				} break;
				case Item::TYPE_MESH: {
					Item::Mesh *mesh = static_cast<Item::Mesh *>(b);

					_new_canvas_batch(); // Meshes can't be batches

					canvas_data.batches[canvas_data.current_batch].base = mesh;
					canvas_data.batches[canvas_data.current_batch].type = Item::TYPE_MESH;
					canvas_data.batches[canvas_data.current_batch].material = mesh->material;

					canvas_data.batches[canvas_data.current_batch].instance_count++;
					index++;
				} break;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, canvas_data.canvas_instance_data_buffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, index * sizeof(CanvasInstanceData), canvas_data.canvas_instance_data);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Actually draw the data

		// Y-sort the batches
		for (uint32_t i = 1; i <= canvas_data.current_batch; i++) {
			for (int k = i; k > 0 && canvas_data.batches[k].ysort < canvas_data.batches[k - 1].ysort; k--) {
				CanvasBatch b = canvas_data.batches[k];
				canvas_data.batches[k] = canvas_data.batches[k - 1];
				canvas_data.batches[k - 1] = b;
			}
		}

		for (uint32_t i = 0; i <= canvas_data.current_batch; i++) {
			RID material = canvas_data.batches[i].material;
			RID texture = canvas_data.batches[i].texture;

			// Bind material UBO
			if (material.is_valid()) {
				// Using custom material
				Material *mat = material_owner.get_or_null(material);

				if (mat->buffer == 0) {
					glGenBuffers(1, &mat->buffer);
					glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_MATERIAL_DATA, mat->buffer);
					utils->allocate_buffer(GL_UNIFORM_BUFFER, sizeof(MaterialData), mat->data, GL_DYNAMIC_DRAW);
				} else {
					glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_MATERIAL_DATA, mat->buffer);
					glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialData), mat->data);
				}
				glBindBuffer(GL_UNIFORM_BUFFER, 0);
			} else {
				Material *defmat = material_owner.get_or_null(default_material);

				glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_MATERIAL_DATA, defmat->buffer);
				glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialData), defmat->data);
			}

			// Bind texture
			if (texture.is_valid()) {
				Texture *tex = texture_owner.get_or_null(texture);

				if (tex) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, tex->texture_buffer);
				} else {
					// Load default texture
					Texture *deftex = texture_owner.get_or_null(default_texture);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, deftex->texture_buffer);
				}
			} else {
				Texture *deftex = texture_owner.get_or_null(default_texture);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, deftex->texture_buffer);
			}

			switch (canvas_data.batches[i].type) {
				case Item::TYPE_RECT: {
					glBindVertexArray(canvas_data.rect_vertex_array);
					glBindBuffer(GL_ARRAY_BUFFER, canvas_data.canvas_instance_data_buffer);
					_enable_attributes(canvas_data.batches[i].start * sizeof(CanvasInstanceData));
					glDrawElementsInstanced(GL_TRIANGLES,
											6,
											GL_UNSIGNED_INT,
											nullptr,
											canvas_data.batches[i].instance_count);

					glBindBuffer(GL_ARRAY_BUFFER, 0);
					glBindVertexArray(0);

					r_data->primitive_count += 2;
				} break;
				case Item::TYPE_MESH: {
					Item::Mesh *m = static_cast<Item::Mesh *>(canvas_data.batches[i].base);
					Mesh *mesh = mesh_owner.get_or_null(m->mesh);

					glBindVertexArray(mesh->vertex_array);
					if (mesh->index_count) {
						glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, nullptr);
					} else {
						glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
					}

					glBindVertexArray(0);

					r_data->primitive_count += mesh->vertex_count / 3;
				} break;
				case Item::TYPE_NONE: {
				} break;
			}

			r_data->draw_calls++;
		}
		// Clear batches when done to prevent excessive rendering
		canvas_data.batches.clear();
		canvas_data.current_batch = 0;
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	glUseProgram(0);
}

void RenderingManagerGL::_prepare_viewport(Viewport *p_viewport) {
	int fbo = p_viewport->framebuffer_id;

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, p_viewport->size.x, p_viewport->size.y);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // TODO: Add a clear colour
	if (p_viewport->texture_depth_id && fbo) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	} else {
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void RenderingManagerGL::_render_viewport(RenderData *r_data, Viewport *p_viewport) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Vector4i rect;
	rect.x = p_viewport->position.x;
	rect.y = p_viewport->position.y;
	rect.z = p_viewport->size.x;
	rect.w = p_viewport->size.y;
	Vector4i screenrect = {0, 0, 1, 1};

	glViewport(rect.x, rect.y, rect.z, rect.w);

	Texture *t = texture_owner.get_or_null(p_viewport->colour_texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t->texture_buffer);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);

	shaders.copy_shader.shader_set_active();

	int offset_size_loc = -1;
	for (int i = 0; i < shaders.copy_shader.uniforms.size(); i++) {
		if (vstring_compare(shaders.copy_shader.uniforms[i].name, "offset_size")) {
			offset_size_loc = shaders.copy_shader.uniforms[i].loc;
			break;
		}
	}
	if (offset_size_loc >= 0) {
		set_uniform_vec4(offset_size_loc, screenrect);
	} else {
		ERR_WARN("Unable to locate the \"offset_size\" shader uniform. Copy shaders may not function correctly.");
	}

	glBindVertexArray(canvas_data.screen_quad_array);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
}

void RenderingManagerGL::_rebuild_viewport_texture(Viewport *p_viewport, Vector2i p_size) {
	// Check if the new size is different to the old
	// If so, destroy and re-build the texture, re-assign it to the framebuffer, done.
	if (p_size == p_viewport->size) {
		return;
	}

	// Destroy existing FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (p_viewport->framebuffer_id != 0) {
		glDeleteFramebuffers(1, &p_viewport->framebuffer_id);
	}

	// Destroy existing texture RIDs
	if (p_viewport->colour_texture.is_valid()) {
		texture_free(p_viewport->colour_texture);
	}

	if (p_viewport->texture_depth_id != 0) {
		glDeleteTextures(1, &p_viewport->texture_depth_id);
	}

	// Create colour texture (can be accessed as a normal texture)
	p_viewport->colour_texture = texture_allocate();
	Texture *colour = texture_owner.get_or_null(p_viewport->colour_texture);
	colour->format = FORMAT_RGBA;

	glGenTextures(1, &colour->texture_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colour->texture_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, p_size.x, p_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Create depth texture (internally managed)
	glGenTextures(1, &p_viewport->texture_depth_id);
	glBindTexture(GL_TEXTURE_2D, p_viewport->texture_depth_id);

	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_DEPTH24_STENCIL8,
				 p_size.x,
				 p_size.y,
				 0,
				 GL_DEPTH_STENCIL,
				 GL_UNSIGNED_INT_24_8,
				 nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	// Create framebuffer
	glGenFramebuffers(1, &p_viewport->framebuffer_id);
	glBindFramebuffer(GL_FRAMEBUFFER, p_viewport->framebuffer_id);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colour->texture_buffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,
						   GL_DEPTH_STENCIL_ATTACHMENT,
						   GL_TEXTURE_2D,
						   p_viewport->texture_depth_id,
						   0);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ERR_FAIL_COND_MSG(status != GL_FRAMEBUFFER_COMPLETE, vformat("Framebuffer returned invalid format %x", status));
	p_viewport->size = p_size;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RID RenderingManagerGL::instance_allocate() {
	RID inst = instance_owner.make_rid();
	Instance *i = instance_owner.get_or_null(inst);
	i->self = inst;
	return inst;
}

void RenderingManagerGL::instance_free(RID p_inst) {
	instance_owner.free(p_inst);
}

void RenderingManagerGL::instance_set_base(RID p_inst, RID p_base) {
	ERR_FAIL_COND_MSG(!p_inst.is_valid(), "Cannot assign a base to an invalid instance.");

	Instance *inst = instance_owner.get_or_null(p_inst);
	ERR_COND_NULL(inst);

	if (p_base.is_valid()) {
		InstanceType type = utils->get_base_type(p_base);
		if (type == INSTANCE_TYPE_NONE) {
			ERR_WARN("Base given did not correspond to a valid instance type.");
		}

		inst->base = p_base;
		inst->type = type;
	} else {
		// Cleanup
		inst->base = RID();
		inst->type = INSTANCE_TYPE_NONE;
	}
}

void RenderingManagerGL::instance_set_transform(RID p_inst, const Transform3D &p_transform) {
	Instance *inst = instance_owner.get_or_null(p_inst);
	ERR_COND_NULL(inst);
	inst->transform = p_transform;
}

RID RenderingManagerGL::mesh_allocate() {
	return mesh_owner.make_rid();
}

void RenderingManagerGL::mesh_free(RID p_rid) {
	if (mesh_owner.owns(p_rid)) {
		Mesh *mesh = mesh_owner.get_or_null(p_rid);

		glDeleteVertexArrays(1, &mesh->vertex_array);
		int buffer_size =
			(mesh->vertex_count * 2 * ((mesh->format & FORMAT_USE_2D_VERTICES) ? 2 : 3) + mesh->vertex_count * 2) *
			sizeof(float);
		if (mesh->format & FORMAT_USE_2D_VERTICES) {
			utils->free_buffer(GL_ARRAY_BUFFER, buffer_size, &mesh->vertex_buffer);
		} else {
			utils->free_buffer(GL_ARRAY_BUFFER, buffer_size, &mesh->vertex_buffer);
		}
		utils->free_buffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count * sizeof(int), &mesh->element_buffer);
		mesh_owner.free(p_rid);
	}
}

void RenderingManagerGL::mesh_set_from_data(RID p_mesh, const MeshData &p_data) {
	Mesh *m = mesh_owner.get_or_null(p_mesh);
	ERR_COND_NULL(m);

	m->index_count = p_data.index_count;
	m->vertex_count = p_data.vertex_count;
	m->format = p_data.format;

	int stride = 0;
	int verts_per_coord = 0;

	// Rebind vertex data to convert from double to single precision
	Vector<float> vertex_data;

	if (p_data.format & FORMAT_USE_2D_VERTICES) {
		stride = 2 * sizeof(float);
		verts_per_coord = 2;

		// Load data for a 2D format (Vector3 will not use the z coordinate)
		for (int i = 0; i < p_data.vertex_data.size(); i++) {
			Vector3 vert = p_data.vertex_data[i];
			vertex_data.push_back(vert.x);
			vertex_data.push_back(vert.y);
		}

		for (int i = 0; i < p_data.uv_data.size(); i++) {
			Vector2 uv = p_data.uv_data[i];
			vertex_data.push_back(uv.x);
			vertex_data.push_back(uv.y);
		}
	} else {
		stride = 3 * sizeof(float);
		verts_per_coord = 3;

		int i;
		for (i = 0; i < p_data.vertex_data.size(); i++) {
			Vector3 vert = p_data.vertex_data[i];
			vertex_data.push_back(vert.x);
			vertex_data.push_back(vert.y);
			vertex_data.push_back(vert.z);
		}

		for (i = 0; i < p_data.normal_data.size(); i++) {
			Vector3 norm = p_data.normal_data[i];
			vertex_data.push_back(norm.x);
			vertex_data.push_back(norm.y);
			vertex_data.push_back(norm.z);
		}

		for (i = 0; i < p_data.uv_data.size(); i++) {
			Vector2 uv = p_data.uv_data[i];
			vertex_data.push_back(uv.x);
			vertex_data.push_back(uv.y);
		}
	}

	glGenVertexArrays(1, &m->vertex_array);
	glBindVertexArray(m->vertex_array);

	glGenBuffers(1, &m->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m->vertex_buffer);
	utils->allocate_buffer(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.ptr(), GL_STATIC_DRAW);

	glGenBuffers(1, &m->element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->element_buffer);
	utils->allocate_buffer(GL_ELEMENT_ARRAY_BUFFER,
						   p_data.index_attribs.size() * sizeof(int),
						   p_data.index_attribs.ptr(),
						   GL_STATIC_DRAW);

	glVertexAttribPointer(0, verts_per_coord, GL_FLOAT, GL_FALSE, stride, nullptr);
	glEnableVertexAttribArray(0);

	// Normals aren't supported by canvas items, disable if they don't exist
	if (p_data.normal_data.size()) {
		glVertexAttribPointer(1, verts_per_coord, GL_FLOAT, GL_FALSE, stride, (void *)int64_t(p_data.normal_offset));
		glEnableVertexAttribArray(1);
	}

	if (p_data.uv_data.size()) {
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)int64_t(p_data.normal_offset * 2));
		glEnableVertexAttribArray(2);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void RenderingManagerGL::mesh_set_material(RID p_mesh, RID p_material) {
	Mesh *m = mesh_owner.get_or_null(p_mesh);
	ERR_COND_NULL(m);
	if (p_material.is_valid()) {
		m->material = p_material;
	} else {
		m->material = RID();
	}
}

/* MATERIAL API */

RID RenderingManagerGL::material_allocate() {
	return material_owner.make_rid();
}

void RenderingManagerGL::material_free(RID p_material) {
	if (material_owner.owns(p_material)) {
		Material *m = material_owner.get_or_null(p_material);
		vdelete(m->data);
		utils->free_buffer(GL_UNIFORM_BUFFER, sizeof(MaterialData), &m->buffer);

		material_owner.free(p_material);
	}
}

void RenderingManagerGL::material_set_colour(RID p_material, const Vector4 &p_colour) {
	Material *m = material_owner.get_or_null(p_material);
	ERR_COND_NULL(m);

	if (!m->data) {
		m->data = vnew(MaterialData);
	}
	GL::Utilities::store_vec4(p_colour, &m->data->diffuse[0]);
	GL::Utilities::store_vec4(p_colour, &m->data->ambient[0]);
	m->data->shininess = 32.0; // TODO: Set this elsewhere
}

void RenderingManagerGL::material_set_specular(RID p_material, const Vector3 &p_specular) {
	Material *m = material_owner.get_or_null(p_material);

	if (!m->data) {
		m->data = vnew(MaterialData);
	}
	GL::Utilities::store_vec3(p_specular, &m->data->specular[0]);
}

void RenderingManagerGL::material_set_texture(RID p_material, RID p_texture) {
	Material *m = material_owner.get_or_null(p_material);
	ERR_COND_NULL(m);

	if (p_texture.is_valid()) {
		m->texture = p_texture;
	} else {
		m->texture = RID();
	}
}

/* Light API */

RID RenderingManagerGL::light_allocate() {
	return light_owner.make_rid();
}

void RenderingManagerGL::light_free(RID p_light) {
	if (light_owner.owns(p_light)) {
		light_owner.free(p_light);
	}
}

void RenderingManagerGL::light_set_type(RID p_light, LightType p_type) {
	Light *l = light_owner.get_or_null(p_light);
	ERR_COND_NULL(l);
	l->type = p_type;
}

void RenderingManagerGL::light_set_ambient(RID p_light, const Vector3 &p_ambient) {
	Light *l = light_owner.get_or_null(p_light);
	ERR_COND_NULL(l);
	l->ambient = p_ambient;
}

void RenderingManagerGL::light_set_diffuse(RID p_light, const Vector3 &p_diffuse) {
	Light *l = light_owner.get_or_null(p_light);
	ERR_COND_NULL(l);
	l->diffuse = p_diffuse;
}

void RenderingManagerGL::light_set_specular(RID p_light, const Vector3 &p_specular) {
	Light *l = light_owner.get_or_null(p_light);
	ERR_COND_NULL(l);
	l->specular = p_specular;
}

void RenderingManagerGL::light_set_range(RID p_light, float p_range) {
	Light *l = light_owner.get_or_null(p_light);
	ERR_COND_NULL(l);
	l->range = p_range;
}

void RenderingManagerGL::light_set_radii(RID p_light, float p_inner_radius, float p_outer_radius) {
	Light *l = light_owner.get_or_null(p_light);
	ERR_COND_NULL(l);

	ERR_FAIL_COND(l->type != TYPE_SPOT_LIGHT);

	l->inner_radius = p_inner_radius;
	l->outer_radius = p_outer_radius;
}

/* Camera API */

RID RenderingManagerGL::camera_allocate() {
	return camera_owner.make_rid();
}

void RenderingManagerGL::camera_free(RID p_rid) {
	if (camera_owner.owns(p_rid)) {
		camera_owner.free(p_rid);
	}
}

void RenderingManagerGL::camera_set_transform(RID p_rid, const Transform3D &p_transform) {
	Camera *c = camera_owner.get_or_null(p_rid);
	ERR_COND_NULL(c);
	c->view = p_transform;
}

void RenderingManagerGL::camera_set_projection(RID p_rid, const Mat4 &p_projection) {
	Camera *c = camera_owner.get_or_null(p_rid);
	ERR_COND_NULL(c);
	c->projection = p_projection;
}

//// 2D/SCENE API

/* CanvasItem API */

RID RenderingManagerGL::item_allocate() {
	RID id = canvas_item_owner.make_rid();
	Item *i = canvas_item_owner.get_or_null(id);
	i->self = id;
	return id;
}

void RenderingManagerGL::item_free(RID p_item) {
	if (canvas_item_owner.owns(p_item)) {
		canvas_item_owner.free(p_item);
	}
}

void RenderingManagerGL::item_set_parent(RID p_item, RID p_parent) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	if (i->parent.is_valid()) {
		Canvas *c = canvas_owner.get_or_null(p_parent);
		ERR_COND_NULL_MSG(c, "Parent RID was valid, but was not a canvas.");

		int idx = c->child_items.find(i);
		ERR_FAIL_COND_MSG(idx == -1, "Canvas was valid, but did not contain the item.");

		c->child_items.remove_at(idx);
	}

	if (p_parent.is_valid()) {
		Canvas *c = canvas_owner.get_or_null(p_parent);
		ERR_COND_NULL_MSG(c, "Parent RID was valid, but was not a canvas.");

		c->child_items.push_back(i);
	}

	i->parent = p_parent;
}

void RenderingManagerGL::item_set_transform(RID p_item, const Transform2D &p_transform) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	ERR_COND_NULL(i);

	i->transform = p_transform;
}

void RenderingManagerGL::item_set_colour(RID p_item, const Vector4 &p_colour) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	ERR_COND_NULL(i);

	i->colour = p_colour;
}

void RenderingManagerGL::item_set_ysort(RID p_item, int p_sort) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	ERR_COND_NULL(i);

	i->ysort = p_sort;
}

void RenderingManagerGL::item_set_rect(RID p_item, const Vector2 &p_position, const Vector2 &p_size) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	ERR_COND_NULL(i);

	if (i->base && i->base->type != Item::TYPE_RECT) {
		i->~Item();
	}

	i->alloc_base<Item::Rect>();
	Item::Rect *r = static_cast<Item::Rect *>(i->base);
	r->rect_offset = p_position;
	r->rect_size = p_size;
	i->flags |= Item::ITEM_FLAG_USE_RECT;
}

void RenderingManagerGL::item_set_rect_offset(RID p_item, const Vector2 &p_offset) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	ERR_COND_NULL(i);
	ERR_FAIL_COND_MSG((i->base == nullptr) || i->base->type != Item::TYPE_RECT, "Item was not a Rect");

	Item::Rect *r = static_cast<Item::Rect *>(i->base);
	r->rect_offset = p_offset;
}

void RenderingManagerGL::item_set_flag(RID p_item, ItemFlag p_flag) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	ERR_COND_NULL(i);

	if (!i->base) {
		i->alloc_base<Item::Base>();
	}

	i->flags |= p_flag;
}

void RenderingManagerGL::item_set_texture_rect(RID p_item,
											   RID p_texture,
											   const Vector2 &p_position,
											   const Vector2 &p_size) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	ERR_COND_NULL(i);

	if (!i->base) {
		i->alloc_base<Item::Rect>();
	}

	if (i->base->type != Item::TYPE_RECT) {
		i->~Item();
		i->alloc_base<Item::Rect>();
	}

	Item::Rect *r = static_cast<Item::Rect *>(i->base);
	ERR_COND_NULL(r);

	Texture *t = texture_owner.get_or_null(p_texture);
	ERR_COND_NULL(t);
	if (t->format == FORMAT_R) {
		i->flags |= Item::ITEM_FLAG_ALPHA_ONLY;
	}

	if (t->use_sdf) {
		i->flags |= Item::ITEM_FLAG_USE_SDF;
	}

	r->texture = p_texture;
	r->rect_offset = p_position;
	r->rect_size = p_size;
	i->flags |= Item::ITEM_FLAG_USE_RECT;
}

void RenderingManagerGL::item_set_uv_rect(RID p_item, Vector2 p_offset, Vector2 p_size) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	ERR_COND_NULL(i);

	if (i->base && i->base->type == Item::TYPE_RECT) {
		Item::Rect *r = static_cast<Item::Rect *>(i->base);
		i->flags |= Item::ITEM_FLAG_USE_TEXTURE_SUBCOORDS;
		r->uv_offset = p_offset;
		r->uv_size = p_size; // Offset and size are relative to the texture
		return;
	} else if (i->base) {
		i->~Item();
	}

	i->alloc_base<Item::Rect>();
	Item::Rect *r = static_cast<Item::Rect *>(i->base);
	i->flags |= Item::ITEM_FLAG_USE_TEXTURE_SUBCOORDS;
	r->uv_offset = p_offset;
	r->uv_size = p_size;
}

void RenderingManagerGL::item_set_mesh(RID p_item, RID p_mesh) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	Mesh *mesh = mesh_owner.get_or_null(p_mesh);
	ERR_COND_NULL(i);

	if (i->base && i->base->type == Item::TYPE_MESH) {
		Item::Mesh *m = static_cast<Item::Mesh *>(i->base);
		m->mesh = p_mesh;
		m->material = mesh->material;
		return;
	} else if (i->base) {
		// Destroy data
		i->~Item();
	}

	i->alloc_base<Item::Mesh>();
	Item::Mesh *m = static_cast<Item::Mesh *>(i->base);
	m->mesh = p_mesh;
	m->material = mesh->material;
}

void RenderingManagerGL::item_set_material(RID p_item, RID p_material) {
	Item *i = canvas_item_owner.get_or_null(p_item);
	ERR_COND_NULL(i);

	if (!i->base) {
		i->alloc_base<Item::Base>();
	}

	i->base->material = p_material;
}

RID RenderingManagerGL::canvas_allocate() {
	return canvas_owner.make_rid();
}

void RenderingManagerGL::canvas_free(RID p_canvas) {
	if (canvas_owner.owns(p_canvas)) {
		canvas_owner.free(p_canvas);
	}
}

RID RenderingManagerGL::texture_allocate() {
	return texture_owner.make_rid();
}

void RenderingManagerGL::texture_free(RID p_texture) {
	if (texture_owner.owns(p_texture)) {
		Texture *t = texture_owner.get_or_null(p_texture);
		glDeleteTextures(1, &t->texture_buffer);
		texture_owner.free(p_texture);
	}
}

void RenderingManagerGL::texture_set_from_data(RID p_texture,
											   const uint8_t *p_data,
											   int p_width,
											   int p_height,
											   TextureFormat p_format,
											   TextureMask p_mask) {
	Texture *tex = texture_owner.get_or_null(p_texture);
	ERR_COND_NULL(tex);

	uint32_t extern_format = GL_RED;
	tex->format = p_format;

	switch (p_format) {
		case FORMAT_R: {
			extern_format = GL_RED;
		} break;
		case FORMAT_RGB: {
			extern_format = GL_RGB;
		} break;
		case FORMAT_RGBA: {
			extern_format = GL_RGBA;
		} break;
	}

	// Make pixel data align per-byte to prevent alignment issues from the 4-byte default
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &tex->texture_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex->texture_buffer);

	// First, because of position in the mask
	int mipmap_type = (p_mask & 0x0000003f);
	int mipmaps = 0;
	switch (mipmap_type) {
		case MASK_FILTER_LINEAR:
			mipmaps = GL_LINEAR;
			break;
		case MASK_FILTER_NEAREST:
			mipmaps = GL_NEAREST;
			break;
		default:
			mipmaps = GL_LINEAR;
			break;
	}

	int repeat_type = (p_mask & 0xffffffc0);
	int wrap = 0;
	switch (repeat_type) {
		case MASK_WRAP_CLAMP_TO_BORDER:
			wrap = GL_CLAMP_TO_BORDER;
			break;
		case MASK_WRAP_CLAMP_TO_EDGE:
			wrap = GL_CLAMP_TO_EDGE;
			break;
		case MASK_WRAP_REPEAT:
			wrap = GL_REPEAT;
			break;
		default:
			wrap = GL_REPEAT;
			break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D,
					GL_TEXTURE_MIN_FILTER,
					(p_mask == MASK_DEFAULT) ? GL_LINEAR_MIPMAP_LINEAR : mipmaps);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mipmaps);

	glTexImage2D(GL_TEXTURE_2D, 0, extern_format, p_width, p_height, 0, extern_format, GL_UNSIGNED_BYTE, p_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

RID RenderingManagerGL::viewport_allocate() {
	RID rid = viewport_owner.make_rid();
	Viewport *v = viewport_owner.get_or_null(rid);
	v->self = rid;
	return rid;
}

void RenderingManagerGL::viewport_free(RID p_viewport) {
	if (viewport_owner.owns(p_viewport)) {
		viewport_owner.free(p_viewport);
	}
}

void RenderingManagerGL::viewport_set_position(RID p_viewport, Vector2i p_position) {
	Viewport *v = viewport_owner.get_or_null(p_viewport);
	ERR_COND_NULL(v);
	v->position = p_position;
}

void RenderingManagerGL::viewport_set_size(RID p_viewport, Vector2i p_size) {
	Viewport *v = viewport_owner.get_or_null(p_viewport);
	ERR_COND_NULL(v);
	_rebuild_viewport_texture(v, p_size);
}

void RenderingManagerGL::viewport_set_window(RID p_viewport, int p_window_id) {
	Viewport *v = viewport_owner.get_or_null(p_viewport);
	ERR_COND_NULL(v);
	v->viewport_to_window = p_window_id;
}

void RenderingManagerGL::viewport_attach_camera(RID p_viewport, RID p_camera) {
	Viewport *v = viewport_owner.get_or_null(p_viewport);
	ERR_COND_NULL(v);
	v->camera = p_camera;
}

void RenderingManagerGL::viewport_set_parent(RID p_viewport, RID p_parent) {
	Viewport *v = viewport_owner.get_or_null(p_viewport);
	ERR_COND_NULL(v);
	v->parent = p_parent;
}

void RenderingManagerGL::viewport_set_active(RID p_viewport, bool p_active) {
	Viewport *v = viewport_owner.get_or_null(p_viewport);
	ERR_COND_NULL(v);

	v->active = p_active;
	if (p_active && active_viewports.find(v) == -1) {
		active_viewports.push_back(v);
		sorted_viewports_dirty = true;
	}
}

void RenderingManagerGL::viewport_attach_canvas(RID p_viewport, RID p_canvas) {
	Viewport *v = viewport_owner.get_or_null(p_viewport);
	ERR_COND_NULL(v);
	v->canvas = p_canvas;
}

RID RenderingManagerGL::viewport_get_texture(RID p_viewport) {
	Viewport *v = viewport_owner.get_or_null(p_viewport);
	ERR_COND_NULL_R(v, RID());
	return v->colour_texture;
}

void RenderingManagerGL::texture_use_sdf(RID p_texture, bool p_value) {
	Texture *t = texture_owner.get_or_null(p_texture);
	ERR_COND_NULL(t);
	t->use_sdf = p_value;
}

void RenderingManagerGL::free(RID p_rid) {
	if (instance_owner.owns(p_rid)) {
		instance_owner.free(p_rid);
	} else if (mesh_owner.owns(p_rid)) {
		mesh_owner.free(p_rid);
	} else if (camera_owner.owns(p_rid)) {
		camera_owner.free(p_rid);
	} else if (light_owner.owns(p_rid)) {
		light_owner.free(p_rid);
	} else if (canvas_item_owner.owns(p_rid)) {
		canvas_item_owner.free(p_rid);
	} else {
		ERR_FAIL_COND_MSG(p_rid.is_valid() == true,
						  "Could not free the given RID as its RID does not correspond to any current RID owners.");
	}
}

void RenderingManagerGL::_enable_attributes(uint32_t p_start, uint32_t p_rate) {
	for (int i = 4; i < 9; i++) {
		glBindVertexArray(canvas_data.rect_vertex_array);

		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i,
							  4,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(CanvasInstanceData),
							  (void *)(p_start + (i - 4) * 4 * sizeof(float)));
		// TODO: Temporary fix
		if (glVertexAttribDivisor) {
			glVertexAttribDivisor(i, p_rate);
		}
	}
}

/**
 * @brief Creates a new batch to manage instance information.
 */
void RenderingManagerGL::_new_canvas_batch() {
	if (canvas_data.batches.size() == 0) {
		CanvasBatch cb;
		canvas_data.batches.push_back(cb);
		return;
	}

	// Don't create a new batch when there isn't any data applied to the current one
	if (canvas_data.batches[canvas_data.current_batch].instance_count == 0) {
		return;
	}

	CanvasBatch cb = canvas_data.batches[canvas_data.current_batch];
	cb.start = canvas_data.batches[canvas_data.current_batch].start +
			   canvas_data.batches[canvas_data.current_batch].instance_count;
	cb.instance_count = 0;
	canvas_data.batches.push_back(cb);
	canvas_data.current_batch++;
}

RenderingManagerGL::RenderingManagerGL() {
	print_verbose("Registering OpenGL rendering server");
	// Load GLAD function pointers here
	bool is_egl = eglGetProcAddress != nullptr;
	bool glad_loaded = false;

	if (is_egl && !glad_loaded) {
		if (is_gles_over_gl()) {
			glad_loaded = gladLoadGLES2((GLADloadfunc)_egl_load_function_wrapper);
		} else {
			glad_loaded = gladLoadGL((GLADloadfunc)_egl_load_function_wrapper);
		}
	}

	if (!glad_loaded) {
		if (is_gles_over_gl()) {
			glad_loaded = gladLoaderLoadGLES2();
		} else {
			glad_loaded = gladLoaderLoadGL();
		}
	}
	ERR_FAIL_COND_MSG(!glad_loaded, "GLAD was unable to be loaded.");

	utils = vnew(GL::Utilities);
	stbi_set_flip_vertically_on_load(true);

	// Query device info
	String vendor = (const char *)glGetString(GL_VENDOR);
	String renderer = (const char *)glGetString(GL_RENDERER);

	OS::get_singleton()->set_device_name(renderer.remove_suffix("/PCIe/SSE2"));
	OS::get_singleton()->set_device_vendor(vendor.remove_suffix(" Corporation"));

	// Check for debug output.
	// We will also check for extension strings here, as it's the least expensive to do at once

	if (OS::get_singleton()->is_stdout_verbose()) {
		if (is_gles_over_gl()) {
			PFNDEBUGMESSAGECALLBACK callback = (PFNDEBUGMESSAGECALLBACK)eglGetProcAddress("glDebugMessageCallback");
			if (!callback) {
				callback = (PFNDEBUGMESSAGECALLBACK)eglGetProcAddress("glDebugMessageCallbackKHR");
			}

			if (callback) {
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				callback((GLDEBUGPROC)Utilities::debug_message_callback, nullptr);
				glEnable(GL_DEBUG_OUTPUT);
			}
		} else {
			if (GLAD_GL_KHR_debug) {
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback((GLDEBUGPROC)Utilities::debug_message_callback, nullptr);
				glEnable(GL_DEBUG_OUTPUT);

				glDebugMessageControl(GL_DEBUG_SOURCE_API,
									  GL_DEBUG_TYPE_ERROR,
									  GL_DEBUG_SEVERITY_HIGH,
									  0,
									  nullptr,
									  GL_TRUE);
				glDebugMessageControl(GL_DEBUG_SOURCE_API,
									  GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
									  GL_DEBUG_SEVERITY_HIGH,
									  0,
									  nullptr,
									  GL_TRUE);
				glDebugMessageControl(GL_DEBUG_SOURCE_API,
									  GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
									  GL_DEBUG_SEVERITY_HIGH,
									  0,
									  nullptr,
									  GL_TRUE);
				glDebugMessageControl(GL_DEBUG_SOURCE_API,
									  GL_DEBUG_TYPE_PORTABILITY,
									  GL_DEBUG_SEVERITY_HIGH,
									  0,
									  nullptr,
									  GL_TRUE);
				glDebugMessageControl(GL_DEBUG_SOURCE_API,
									  GL_DEBUG_TYPE_PERFORMANCE,
									  GL_DEBUG_SEVERITY_HIGH,
									  0,
									  nullptr,
									  GL_TRUE);
				glDebugMessageControl(GL_DEBUG_SOURCE_API,
									  GL_DEBUG_TYPE_OTHER,
									  GL_DEBUG_SEVERITY_HIGH,
									  0,
									  nullptr,
									  GL_TRUE);
			} else {
				print_line("OpenGL debugging is not supported.");
			}
		}
	}

	// Setup lights

	// Directional lights
	scene_data.directional_lights =
		(SceneData::DirectionalLight *)Memory::vallocate(sizeof(SceneData::DirectionalLight) * 32);
	Memory::vzero(scene_data.directional_lights, sizeof(SceneData::DirectionalLight) * 32);
	glGenBuffers(1, &scene_data.directional_light_buffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_DIRECTIONAL_LIGHT_DATA, scene_data.directional_light_buffer);
	utils->allocate_buffer(GL_UNIFORM_BUFFER, sizeof(SceneData::DirectionalLight) * 32, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	print_verbose("Allocated 32 directional lights to the scene");

	// Point lights
	scene_data.point_lights = (SceneData::PointLight *)Memory::vallocate(sizeof(SceneData::PointLight) * 32);
	Memory::vzero(scene_data.point_lights, sizeof(SceneData::PointLight) * 32);
	glGenBuffers(1, &scene_data.point_light_buffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_LIGHT_DATA, scene_data.point_light_buffer);
	utils->allocate_buffer(GL_UNIFORM_BUFFER, sizeof(SceneData::PointLight) * 32, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	print_verbose("Allocated 32 point lights to the scene");

	// Spot lights
	scene_data.spot_lights = (SceneData::SpotLight *)Memory::vallocate(sizeof(SceneData::SpotLight) * 32);
	Memory::vzero(scene_data.spot_lights, sizeof(SceneData::SpotLight) * 32);
	glGenBuffers(1, &scene_data.spot_light_buffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_SPOT_LIGHT_DATA, scene_data.spot_light_buffer);
	utils->allocate_buffer(GL_UNIFORM_BUFFER, sizeof(SceneData::SpotLight) * 32, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	print_verbose("Allocated 32 spot lights to the scene");

	// Material setup

	print_verbose("Setting up the default material");
	// Setup default material
	default_material = material_allocate();
	material_set_colour(default_material, Vector4(1, 1, 1, 1));
	material_set_specular(default_material, Vector3(0, 0, 0));
	Material *mat = material_owner.get_or_null(default_material);
	glGenBuffers(1, &mat->buffer);
	glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_MATERIAL_DATA, mat->buffer);
	utils->allocate_buffer(GL_UNIFORM_BUFFER, sizeof(MaterialData), mat->data, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	print_verbose("Setting up the default texture");
	// Create default texture (a 1x1 white pixel)
	default_texture = texture_allocate();
	Texture *t = texture_owner.get_or_null(default_texture);
	Vector<uint8_t> data = {255, 255, 255};

	glGenTextures(1, &t->texture_buffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t->texture_buffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data.ptrw());
	glBindTexture(GL_TEXTURE_2D, 0);

	material_set_texture(default_material, default_texture);

	print_verbose("Setting up default rect");
	{
		glGenVertexArrays(1, &canvas_data.rect_vertex_array);
		glBindVertexArray(canvas_data.rect_vertex_array);

		/* clang-format off */
        float rect[16] = {
            0, 0,
            1, 1,
            0, 1,
            1, 0,
            0, 0, // uv data
            1, 1,
            0, 1,
            1, 0
        };
		/* clang-format on */

		glGenBuffers(1, &canvas_data.rect_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, canvas_data.rect_vertex_buffer);
		utils->allocate_buffer(GL_ARRAY_BUFFER, 16 * sizeof(float), rect, GL_STATIC_DRAW);

		uint32_t indicies[6] = {2, 1, 0, 3, 0, 1};
		glGenBuffers(1, &canvas_data.rect_index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, canvas_data.rect_index_buffer);
		utils->allocate_buffer(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(uint32_t), indicies, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(8 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	print_verbose("Creating screen quad");
	{
		glGenBuffers(1, &canvas_data.screen_quad);
		glBindBuffer(GL_ARRAY_BUFFER, canvas_data.screen_quad);

		/* clang-format off */
		float screen_quad[12] = {
			-1.0f,
			-1.0f,
			1.0f,
			-1.0f,
			1.0f,
			1.0f,
			-1.0f,
			-1.0f,
			1.0f,
			1.0f,
			-1.0f,
			1.0f,
		};
		/* clang-format on */

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, screen_quad, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &canvas_data.screen_quad_array);
		glBindVertexArray(canvas_data.screen_quad_array);
		glBindBuffer(GL_ARRAY_BUFFER, canvas_data.screen_quad);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Allocate canvas instance UBO
	print_verbose("Creating CanvasInstanceData UBO");
	canvas_data.canvas_instance_data =
		(CanvasInstanceData *)Memory::vallocate(sizeof(CanvasInstanceData) * MAX_INSTANCE_DATA_COUNT);
	Memory::vzero(canvas_data.canvas_instance_data, sizeof(CanvasInstanceData) * MAX_INSTANCE_DATA_COUNT);
	glGenBuffers(1, &canvas_data.canvas_instance_data_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, canvas_data.canvas_instance_data_buffer);
	utils->allocate_buffer(GL_ARRAY_BUFFER, sizeof(CanvasInstanceData) * MAX_INSTANCE_DATA_COUNT, GL_STREAM_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	OS::get_singleton()->print("OpenGL v3.3.0 - using device %s", OS::get_singleton()->get_device_name().get_data());
}

RenderingManagerGL::~RenderingManagerGL() {
	vdelete(utils);
}
