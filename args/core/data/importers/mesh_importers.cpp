#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <core/data/importers/mesh_importers.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>

#include <map>

#include "core/logging/logging.hpp"

namespace args::core
{
    common::result_decay_more<filesystem::basic_resource, fs_error> obj_mesh_loader::load(const filesystem::basic_resource& resource, mesh_import_settings&& settings)
    {
        using common::Err, common::Ok;
        // decay overloads the operator of ok_type and operator== for valid_t.
        using decay = common::result_decay_more<filesystem::basic_resource, fs_error>;

        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig config;

        config.triangulate = settings.triangulate;
        config.vertex_color = settings.vertex_color;

        if (!reader.ParseFromString(resource.to_string(), "", config))
        {
            return decay(Err(args_fs_error(reader.Error().c_str())));
        }

        if (!reader.Warning().empty())
            log::warn(reader.Warning().c_str());

        tinyobj::attrib_t attributes = reader.GetAttrib();
        std::vector<tinyobj::shape_t> shapes = reader.GetShapes();

        mesh data;

        struct vtx_data
        {
            math::vec3 vert;
            math::vec3 norm;
            math::vec2 uv;

            bool operator==(const vtx_data& other)
            {
                return vert.x == other.vert.x && vert.y == other.vert.y && vert.z == other.vert.z;
            }
        };

        std::vector<vtx_data> vertices;

        for (auto& shape : shapes)
        {
            sub_mesh submesh;
            submesh.name = shape.name;
            submesh.indexOffset = data.indices.size();
            submesh.indexCount = shape.mesh.indices.size();

            for (auto& indexData : shape.mesh.indices)
            {
                uint vtxIdx = indexData.vertex_index * 3;

                vtx_data vtx = { { attributes.vertices[vtxIdx + 0], attributes.vertices[vtxIdx + 1], attributes.vertices[vtxIdx + 2] },
                    { 0.f, 0.f, 0.f },
                    { 0.f, 0.f } };

                size_type index = vertices.size();

                for (size_type i = 0; i < vertices.size(); i++)
                    if (vertices[i] == vtx)
                    {
                        index = i;
                        break;
                    }

                if (index == vertices.size())
                {
                    vertices.push_back(vtx);
                    data.vertices.push_back(vtx.vert);
                    data.normals.push_back(vtx.norm);
                    data.uvs.push_back(vtx.uv);
                }

                data.indices.push_back(index);
            }

            data.submeshes.push_back(submesh);
        }

        mesh::calculate_tangents(&data);

        filesystem::basic_resource result(nullptr);

        mesh::to_resource(&result, data);

        return decay(Ok(result));
    }
}
