#if !defined(DOXY_EXCLUDE)
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#endif

#include <core/data/importers/mesh_importers.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>
#include <core/common/string_extra.hpp>
#include <unordered_map>

namespace legion::core::detail
{
    // Utility hash class for hashing all the vertex data.
    struct vertex_hash
    {
        id_type hash;
        vertex_hash(math::vec3 vertex, math::color color, math::vec3 normal, math::vec2 uv)
        {
            std::hash<math::vec3> vec3Hasher;
            std::hash<math::color> colorHasher;
            std::hash<math::vec2> vec2Hasher;
            hash = 0;
            math::detail::hash_combine(hash, vec3Hasher(vertex));
            math::detail::hash_combine(hash, colorHasher(color));
            math::detail::hash_combine(hash, vec3Hasher(normal));
            math::detail::hash_combine(hash, vec2Hasher(uv));
        }

        bool operator==(const vertex_hash& other) const
        {
            return hash == other.hash;
        }
        bool operator!=(const vertex_hash& other) const
        {
            return hash != other.hash;
        }
    };
}

#if !defined(DOXY_EXCLUDE)
namespace std
{
    template<>
    struct hash<legion::core::detail::vertex_hash>
    {
        size_t operator()(legion::core::detail::vertex_hash const& vh) const
        {
            return vh.hash;
        }
    };
}
#endif

namespace legion::core
{
    common::result_decay_more<mesh, fs_error> obj_mesh_loader::load(const filesystem::basic_resource& resource, mesh_import_settings&& settings)
    {
        using common::Err, common::Ok;
        // decay overloads the operator of ok_type and operator== for valid_t.
        using decay = common::result_decay_more<mesh, fs_error>;

        // tinyobj objects
        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig config;

        // Configure settings.
        config.triangulate = settings.triangulate;
        config.vertex_color = settings.vertex_color;

        std::string mtl = "newmtl None\n\
            Ns 0\n\
            Ka 0.000000 0.000000 0.000000\n\
            Kd 0.8 0.8 0.8\n\
            Ks 0.8 0.8 0.8\n\
            d 1\n\
            illum 2\n\0";

        if (settings.materialFile.get_virtual_path() != std::string(""))
        {
            auto result = settings.materialFile.get();
            if (result != common::valid)
                log::warn("{}", result.get_error());
            else
            {
                filesystem::basic_resource resource = result;
                mtl = resource.to_string();
            }
        }

        // Try to parse the mesh data from the text data in the file.
        if (!reader.ParseFromString(resource.to_string(), mtl, config))
        {
            return decay(Err(legion_fs_error(reader.Error().c_str())));
        }

        // Print any warnings.
        if (!reader.Warning().empty())
        {
            std::string warnings = reader.Warning();
            common::replace_items(warnings, "\n", " ");
            log::warn(warnings.c_str());
        }

        // Get all the vertex and composition data.
        tinyobj::attrib_t attributes = reader.GetAttrib();
        std::vector<tinyobj::shape_t> shapes = reader.GetShapes();

        // Create the mesh
        mesh data;

        // Sparse map like constructs to map both vertices and indices.
        std::vector<detail::vertex_hash> vertices;
        std::unordered_map<detail::vertex_hash, size_type> indices;

        // Iterate submeshes.
        for (auto& shape : shapes)
        {
            sub_mesh submesh;
            submesh.name = shape.name;
            submesh.indexOffset = data.indices.size();
            submesh.indexCount = shape.mesh.indices.size();

            for (auto& indexData : shape.mesh.indices)
            {
                // Get the indices into the tinyobj attributes.
                uint vertexIndex = indexData.vertex_index * 3;
                uint normalIndex = indexData.normal_index * 3;
                uint uvIndex = indexData.texcoord_index * 2;

                // Extract the actual vertex data. (We flip the X axis to convert it to our left handed coordinate system.)
                math::vec3 vertex(-attributes.vertices[vertexIndex + 0], attributes.vertices[vertexIndex + 1], attributes.vertices[vertexIndex + 2]);

                math::color color = math::colors::white;
                if (vertexIndex + 2 < attributes.colors.size())
                    color = math::color(attributes.colors[vertexIndex + 0], attributes.colors[vertexIndex + 1], attributes.colors[vertexIndex + 2]);

                math::vec3 normal(-attributes.normals[normalIndex + 0], attributes.normals[normalIndex + 1], attributes.normals[normalIndex + 2]);
                math::vec2 uv{};
                if (uvIndex + 1 < attributes.texcoords.size())
                    uv = math::vec2(attributes.texcoords[uvIndex + 0], attributes.texcoords[uvIndex + 1]);

                // Create a hash to check for doubles.
                detail::vertex_hash hash(vertex, color, normal, uv);

                // Use the properties of sparse containers to check for duplicate items.
                if (indices[hash] >= vertices.size() || vertices[indices[hash]] != hash)
                {
                    // Insert new hash into sparse container.
                    indices[hash] = vertices.size();
                    vertices.push_back(hash);

                    // Append vertex data.
                    data.vertices.push_back(vertex);
                    data.colors.push_back(color);
                    data.normals.push_back(normal);
                    data.uvs.push_back(uv);
                }

                // Append the index of the newly added vertex or whichever one was added earlier.
                data.indices.push_back(indices[hash]);
            }

            // Add the sub-mesh to the mesh.
            data.submeshes.push_back(submesh);
        }

        // Because we only flip one axis we also need to flip the triangle rotation.
        for (int i = 0; i < data.indices.size(); i += 3)
        {
            uint i1 = data.indices[i + 1];
            uint i2 = data.indices[i + 2];
            data.indices[i + 1] = i2;
            data.indices[i + 2] = i1;
        }

        // Calculate the tangents.
        mesh::calculate_tangents(&data);

        // Construct and return the result.
        return decay(Ok(data));
    }
}
