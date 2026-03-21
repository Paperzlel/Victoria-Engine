#pragma once

#include "core/math/transform_3d.h"
#include "core/typedefs.h"

struct RenderData {
	u64 draw_calls;
	u64 primitive_count;

	double render_time;

	Transform3D projection_matrix;
	Mat4 screen_matrix;
	Mat4 scene_matrix; // TODO: Move this to viewport rendering - camera info should be there instead.

	Transform3D camera_matrix;
	Transform3D inv_camera_matrix;
};