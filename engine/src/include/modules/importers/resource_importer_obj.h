#pragma once

#include "core/io/resource_format_importer.h"
#include "core/math/vector2.h"
#include "core/math/vector3.h"
#include "core/typedefs.h"

class ResourceFormatImporterOBJ : public ResourceFormatImporter {
private:
	struct Vertex {
		Vector3 vertex;
		Vector3 normal;
		Vector2 uv;

		bool operator==(const Vertex &p_other) const;
	};

	Vector<Vertex> vertex_array;
	Vector<int> index_array;

	void index();

public:
	virtual Ref<Resource> _import(const String &p_file, int p_argc, Variant *p_args) override;

	ResourceFormatImporterOBJ();
};