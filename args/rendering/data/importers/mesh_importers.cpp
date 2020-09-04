#define TINYOBJLOADER_IMPLEMENTATION
#include <rendering/detail/tiny_obj_loader.h>

#include <rendering/data/importers/mesh_importers.hpp>

#include <map>

namespace args::rendering
{
    common::result_decay_more<fs::basic_resource, fs_error> obj_mesh_loader::load(const fs::basic_resource& resource, mesh_import_settings&& settings)
    {
        using common::Err, common::Ok;
        // decay overloads the operator of ok_type and operator== for valid_t.
        using decay = common::result_decay_more<fs::basic_resource, fs_error>;

        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig config;

        config.triangulate = settings.triangulate;
        config.vertex_color = settings.vertex_color;

        if (!reader.ParseFromString(resource.to_string(), "", config))
        {
            return decay(Err(args_fs_error(reader.Error().c_str())));
        }

        if (!reader.Warning().empty())
            std::cout << reader.Warning() << std::endl;

        tinyobj::attrib_t attributes = reader.GetAttrib();
        std::vector<tinyobj::shape_t> shapes = reader.GetShapes();

        mesh_data data;

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
            submesh_data submesh;
            submesh.name = shape.name;
            submesh.indexOffset = data.indices.size();
            submesh.indexCount = shape.mesh.indices.size();

            for (auto& indexData : shape.mesh.indices)
            {
                //uint idx = data.vertices.size();
                //submesh.indices.push_back(idx);
                //data.vertices.push_back({ attributes.vertices[indexData.vertex_index + 0], attributes.vertices[indexData.vertex_index + 1], attributes.vertices[indexData.vertex_index + 2] });
                //data.vertices[idx] = (data.vertices[idx] + math::vec3(1.f, 1.f, 1.f)) / 2.f;
                //data.normals.push_back(math::vec3());
                //data.uvs.push_back(math::vec2());

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

        mesh_data::calculate_tangents(&data);

        fs::basic_resource result(nullptr);

        mesh_data::to_resource(&result, data);

        return decay(Ok(result));
    }
}
