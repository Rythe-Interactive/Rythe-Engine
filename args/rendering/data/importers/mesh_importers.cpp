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


        return decay(Ok(fs::basic_resource(nullptr)));
    }
}
