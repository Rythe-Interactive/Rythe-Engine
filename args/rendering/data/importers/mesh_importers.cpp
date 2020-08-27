#define TINYOBJLOADER_IMPLEMENTATION
#include <rendering/data/detail/tiny_obj_loader.h>

#include <rendering/data/importers/mesh_importers.hpp>

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

        for (int i = 0; i < attributes.vertices.size() / 3; i++)
        {
            int ind3 = i * 3;
            int ind2 = i * 2;
            data.vertices.push_back({ attributes.vertices[ind3 + 0], attributes.vertices[ind3 + 1] , attributes.vertices[ind3 + 2] });
            data.normals.push_back({ attributes.normals[ind3 + 0], attributes.normals[ind3 + 1] , attributes.normals[ind3 + 2] });
            data.uvs.push_back({ attributes.texcoords[ind2 + 0], attributes.texcoords[ind2 + 1] });
        }

        mesh_data::calculate_tangents(&data);

        for (auto& shape : shapes)
        {
            submesh_data submesh;
            submesh.name = shape.name;
            for (auto& index : shape.mesh.indices)
                submesh.indices.push_back(index.vertex_index);

            data.submeshes.push_back(submesh);
        }

        fs::basic_resource result(nullptr);

        mesh_data::to_resource(&result, data);

        return decay(Ok(result));
    }
}
