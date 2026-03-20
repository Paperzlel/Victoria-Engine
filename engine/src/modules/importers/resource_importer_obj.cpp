#include "modules/importers/resource_importer_obj.h"

#include "core/io/filesystem.h"
#include "core/math/vector2.h"
#include "core/variant/variant.h"

#include "scene/resources/mesh.h"

bool ResourceFormatImporterOBJ::Vertex::operator==(const Vertex &p_other) const {
    if (vertex != p_other.vertex) {
        return false;
    }
    if (normal != p_other.normal) {
        return false;
    }
    if (uv != p_other.uv) {
        return false;
    }

    return true;
}

/**
 * @brief Simplifies the vertex data by trimming out the previous duplicate vertices, and obtains the index array for the mesh.
 */
void ResourceFormatImporterOBJ::index() {
    Vector<Vertex> new_vertex_array;

    int max_idx = 0;
    for (const Vertex &v : vertex_array) {
        int idx = new_vertex_array.find(v);
        if (idx == -1) {
            new_vertex_array.push_back(v);
            index_array.push_back(max_idx);

            max_idx++;
        } else {
            index_array.push_back(idx);
        }
    }

    vertex_array = new_vertex_array;
}

Ref<Resource> ResourceFormatImporterOBJ::_import(const String &p_path, int p_argc, Variant *p_args) {
    // Clear out data when no longer needed
    vertex_array.clear();
    index_array.clear();
    Ref<FileSystem> fs = FileSystem::open(p_path, FileSystem::FILE_ACCESS_READ);

    // Invalid file contents
    if (!fs->is_valid_file()) {
        //return data;
    }

    Vector<Vector3> vertices;
    Vector<Vector3> normals;
    Vector<Vector2> uvs;

    while (fs->get_position() < fs->get_length()) {
        String line = fs->get_line();
        
        // Ignore comments in code
        if (line.begins_with("#")) {
            continue;
        }
        
        // Split the string 
        Vector<String> sline = line.split(" ");

        int comment = sline[sline.size() - 1].find("#");
        if (comment != -1) {
            sline[sline.size() - 1].left(comment);
        }

        if (sline[0] == "v") {
            Vector3 v = {sline[1].to_float(), sline[2].to_float(), sline[3].to_float()};
            vertices.push_back(v);
            continue;
        }
        if (sline[0] == "vn") {
            Vector3 v = {sline[1].to_float(), sline[2].to_float(), sline[3].to_float()};
            normals.push_back(v);
            continue;
        }
        if (sline[0] == "vt") {
            Vector2 v = {sline[1].to_float(), sline[2].to_float()};
            uvs.push_back(v);
            continue;
        }

        //WARNING: We automatically assume that the user has vertex/vertex coords/vertex normals for their face elements. TODO: Note this in the docs.
        if (sline[0] == "f") {
            for (int i = 0; i < 3; i++) {
                Vector<String> substr = sline[i + 1].split("/");
                if (substr.is_empty()) {
                    continue;
                }

                if (substr.size() < 3) {
                    ERR_WARN(vformat("Given vertex indicies (%s) were not in the format v/vt/vn.", sline[i + 1].get_data()));
                    continue;
                }

                int vertex_idx = substr[0].to_int() - 1;
                int uv_idx = substr[1].to_int() - 1;
                int normal_idx = substr[2].to_int() - 1;

                Vertex vert;
                vert.vertex = vertices[vertex_idx];
                vert.normal = normals[normal_idx];
                vert.uv = uvs[uv_idx];

                vertex_array.push_back(vert);
            }

            continue;
        }
    }

    Ref<Mesh> mesh;
    mesh.instantiate();
    index();


    Array a;
    a.resize(Mesh::ARRAY_MAX);
    for (int i = 0; i < Mesh::ARRAY_MAX; i++) {
        switch (i) {
            case Mesh::ARRAY_VERTEX:
            case Mesh::ARRAY_NORMAL: {
                Vector<Vector3> verts;
                verts.resize(vertex_array.size());
                Vector3 *ptrw = verts.ptrw();

                for (int j = 0; j < vertex_array.size(); j++) {
                    const Vertex &v = vertex_array[j];

                    switch (i) {
                        case Mesh::ARRAY_VERTEX: {
                            ptrw[j] = v.vertex;
                        } break;
                        case Mesh::ARRAY_NORMAL: {
                            ptrw[j] = v.normal;
                        }
                    }
                }

                a[i] = verts;
            } break;
            case Mesh::ARRAY_UV: {
                Vector<Vector2> uv_data;
                uv_data.resize(vertex_array.size());
                Vector2 *ptrw = uv_data.ptrw();

                for (int j = 0; j < vertex_array.size(); j++) {
                    const Vertex &v = vertex_array[j];
                    ptrw[j] = v.uv;
                }


                a[i] = uv_data;
            } break;
            case Mesh::ARRAY_INDEX: {
                Vector<int> indicies;
                indicies.resize(index_array.size());
                int *ptrw = indicies.ptrw();

                for (int j = 0; j < index_array.size(); j++) {
                    ptrw[j] = index_array[j];
                }

                a[i] = indicies;
            } break;
            default: {
            }
        }
    }
    mesh->create_from_array(a);

    return mesh;
}

ResourceFormatImporterOBJ::ResourceFormatImporterOBJ() {
    supported_formats.push_back("obj");
}