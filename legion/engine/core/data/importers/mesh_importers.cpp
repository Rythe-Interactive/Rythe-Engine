#if !defined(DOXY_EXCLUDE)
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tinygltf/tiny_gltf.h> 
#endif

#include <core/data/importers/mesh_importers.hpp>
#include <core/math/math.hpp>
#include <core/logging/logging.hpp>
#include <core/common/string_extra.hpp>
#include <unordered_map>
#include <algorithm>

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

    template <class T>
    void handleGltfBuffer(const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView, std::vector<T>* data)
    {
        /*for (size_t i = 0; i < bufferView.byteLength; i += sizeof(T))
        {
            data->push_back(*reinterpret_cast<const T*>(&buffer.data.at(i) + bufferView.byteOffset));
        }*/
        data->resize(bufferView.byteLength / sizeof(T));
        memcpy(data->data(), &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);
    }

    void handleGltfIndices(const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView, std::vector<unsigned int>* data)
    {
        //indices in glft are in uin16
        data->resize(bufferView.byteLength / 2);

        std::vector<int16> origin;
        origin.resize(bufferView.byteLength / 2);
        memcpy(origin.data(), &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);
        //memcpy(data->data(), &buffer.data.at(0) + bufferView.byteOffset, bufferView.byteLength);
        std::copy(origin.begin(), origin.begin()+origin.size(), data->begin());
        //std::copy(buffer.data.at(0) + bufferView.byteOffset, buffer.data.at(0) + bufferView.byteOffset + bufferView.byteLength, std::back_inserter(*data));
    }
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


    common::result_decay_more<mesh, fs_error> gltf_mesh_loader::load(const filesystem::basic_resource& resource, mesh_import_settings&& settings)
    {
        //log::debug("Loading glb");
        using common::Err, common::Ok;
        // decay overloads the operator of ok_type and operator== for valid_t.
        using decay = common::result_decay_more<mesh, fs_error>;

        namespace tg = tinygltf;

        tg::Model model;
        tg::TinyGLTF loader;
        std::string err;
        std::string warn;

        //bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, resource.);
        bool ret = loader.LoadBinaryFromMemory(&model, &err, &warn, resource.data(), resource.size());

        if (!err.empty())
        {
            log::error("Error: {}", err);
        }
        if (!warn.empty())
        {
            log::warn("Warning: {}", warn);
        }
        if (!ret)
        {
            return decay(Err(legion_fs_error("Failed to parse glTF")));
        }

        size_t offset = 0;
        core::mesh meshData;
        for (auto& mesh : model.meshes)
        {
            sub_mesh m;
            m.name = mesh.name;
            for (auto primitive : mesh.primitives)
            {
                for (auto& attrib : primitive.attributes)
                {
                    const tg::Accessor& accessor = model.accessors.at(attrib.second);
                    tg::BufferView& view = model.bufferViews.at(accessor.bufferView);
                    tg::Buffer& buff = model.buffers.at(view.buffer);
                    if (attrib.first.compare("POSITION") == 0)
                    {
                        // Position data
                        detail::handleGltfBuffer<math::vec3>(buff, view, &(meshData.vertices));
                    }
                    else if (attrib.first.compare("NORMAL") == 0)
                    {
                        // Normal data
                        detail::handleGltfBuffer<math::vec3>(buff, view, &(meshData.normals));
                    }
                    else if (attrib.first.compare("TEXCOORD_0") == 0)
                    {
                        // UV data
                        detail::handleGltfBuffer<math::vec2>(buff, view, &(meshData.uvs));
                    }
                    else
                    {
                        log::warn("More data to be found in .gbl. Data can be accesed through: {}", attrib.first);
                    }
                }
                const tg::Accessor& accessor = model.accessors.at(primitive.indices);
                tg::BufferView& view = model.bufferViews.at(accessor.bufferView);
                tg::Buffer& buff = model.buffers.at(view.buffer);
                detail::handleGltfIndices(buff, view, &(meshData.indices));
            }
            m.indexCount = meshData.indices.size() - offset;
            m.indexOffset = meshData.indices.size() - m.indexCount;
            offset += m.indexCount;
            //log::debug("c {}, o {}", m.indexCount, m.indexOffset);
            meshData.submeshes.push_back(m);
        }

        // Debugging purpose
        /*meshData.indices[9] = 4;
        meshData.indices[10] = 6;
        meshData.indices[11] = 7;*/

        /*
        for (int i = 0; i < meshData.vertices.size(); ++i)
        {
            log::debug("[{}]: V{} \tN{} \t{}", i, meshData.vertices[i], meshData.normals[i], meshData.uvs[i]);
            meshData.colors.push_back(core::math::colors::grey);
        }
        for (int i = 0; i < meshData.indices.size(); ++i)
        {
            log::debug("[{}]: I{}", i, meshData.indices[i]);
            if ((i+1) % 6 == 0) log::debug("");
        }
        log::debug("V {}, N {}, UV {}, C {}, I {}", meshData.vertices.size(), meshData.normals.size(), meshData.uvs.size(), meshData.colors.size(), meshData.indices.size());
        */

        for (int i = 0; i < meshData.vertices.size(); ++i)
        {
            meshData.vertices[i] = meshData.vertices[i] * math::vec3(-1, 1, 1);
            meshData.normals[i] = meshData.normals[i] * math::vec3(-1, 1, 1);
            meshData.uvs[i] = meshData.uvs[i] * math::vec2(1, -1);
            meshData.colors.push_back(core::math::colors::grey);
        }

        // Because we only flip one axis we also need to flip the triangle rotation.
        for (int i = 0; i < meshData.indices.size(); i += 3)
        {
            uint i1 = meshData.indices[i + 1];
            uint i2 = meshData.indices[i + 2];
            meshData.indices[i + 1] = i2;
            meshData.indices[i + 2] = i1;
        }

        mesh::calculate_tangents(&meshData);

        return decay(Ok(meshData));
    }
}
