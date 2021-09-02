#include <core/data/loaders/objmeshloader.hpp>

#if !defined(DOXY_EXCLUDE)
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#endif

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
    using base = assets::AssetLoader<mesh>;
    using asset_ptr = base::asset_ptr;
    using import_cfg = base::import_cfg;
    using progress_type = base::progress_type;

    bool ObjMeshLoader::canLoad(const fs::view& file)
    {
        auto result = file.get_extension();
        if (result)
            return result.value() == ".obj";
        return false;
    }

    common::result<asset_ptr> ObjMeshLoader::loadImpl(id_type nameHash, const fs::view& file, const import_cfg& settings, progress_type* progress)
    {
        // tinyobj objects
        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig config;

        // Configure settings.
        config.triangulate = settings.triangulate;
        config.vertex_color = true;

        std::string baseDir = "";
        std::vector<std::string> warnings;

        auto contextPath = file.parent().get_virtual_path();

        filesystem::navigator navigator(contextPath);
        auto solution = navigator.find_solution();
        if (solution.has_error())
            warnings.push_back(std::string("Invalid obj context path, ") + solution.error().what());
        else
        {
            auto s = solution.value();
            if (s.size() != 1)
                warnings.emplace_back("Invalid obj context path, fs::view was not fully local");
            else
            {
                filesystem::basic_resolver* resolver = dynamic_cast<filesystem::basic_resolver*>(s[0].first);
                if (!resolver)
                    warnings.emplace_back("Invalid obj context path, fs::view was not local");
                else
                {
                    resolver->set_target(s[0].second);

                    if (!resolver->is_valid_path())
                        warnings.emplace_back("Invalid obj context path");
                    else
                        baseDir = resolver->get_absolute_path();
                }
            }
        }

        auto result = file.get();

        warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());
        if (!result)
            return { legion_exception_msg(result.error().what()), warnings };

        auto text = result->to_string();

        if (progress)
            progress->advance_progress(5.f);

        tinyobj::MaterialFileReader matFileReader(baseDir);

        // Try to parse the mesh data from the text data in the file.
        if (!reader.ParseFromString(text, matFileReader, config))
        {
            return { legion_fs_error(reader.Error().c_str()), warnings };
        }

        if (!reader.Warning().empty())
        {
            auto readerWarnings = common::split_string_at<'\n'>(reader.Warning());
            warnings.insert(warnings.end(), readerWarnings.begin(), readerWarnings.end());
        }

        if (progress)
            progress->advance_progress(55.f);

        // Create the mesh
        mesh data;

        std::vector<tinyobj::material_t> srcMaterials = reader.GetMaterials();

        const float percentagePerMat = 25.f / static_cast<float>(srcMaterials.size());

        for (auto& srcMat : srcMaterials)
        {
            auto& material = data.materials.emplace_back();
            material.name = srcMat.name;
            material.opaque = math::close_enough(srcMat.dissolve, 1);
            material.alphaCutoff = 0.5f;
            material.doubleSided = false;

            material.albedoValue = math::color(srcMat.diffuse[0], srcMat.diffuse[1], srcMat.diffuse[2]);
            if (!srcMat.diffuse_texname.empty())
            {
                auto imgResult = assets::load<image>(fs::view(contextPath + srcMat.diffuse_texname));

                if (imgResult)
                    material.albedoMap = imgResult.value();
                else
                {
                    warnings.push_back("Unable to load texture \"" + srcMat.diffuse_texname + "\": " + imgResult.error().what());
                    material.albedoMap = assets::invalid_asset<image>;
                }

                warnings.insert(warnings.end(), imgResult.warnings().begin(), imgResult.warnings().end());
            }

            material.metallicValue = srcMat.metallic;
            if (!srcMat.metallic_texname.empty())
            {
                auto imgResult = assets::load<image>(fs::view(contextPath + srcMat.metallic_texname));

                if (imgResult)
                    material.metallicMap = imgResult.value();
                else
                {
                    warnings.push_back("Unable to load texture \"" + srcMat.metallic_texname + "\": " + imgResult.error().what());
                    material.metallicMap = assets::invalid_asset<image>;
                }

                warnings.insert(warnings.end(), imgResult.warnings().begin(), imgResult.warnings().end());
            }

            material.roughnessValue = srcMat.roughness;
            if (!srcMat.roughness_texname.empty())
            {
                auto imgResult = assets::load<image>(fs::view(contextPath + srcMat.roughness_texname));

                if (imgResult)
                    material.roughnessMap = imgResult.value();
                else
                {
                    warnings.push_back("Unable to load texture \"" + srcMat.roughness_texname + "\": " + imgResult.error().what());
                    material.roughnessMap = assets::invalid_asset<image>;
                }

                warnings.insert(warnings.end(), imgResult.warnings().begin(), imgResult.warnings().end());
            }

            material.metallicRoughnessMap = assets::invalid_asset<image>;

            material.emissiveValue = math::color(srcMat.emission[0], srcMat.emission[1], srcMat.emission[2]);
            if (!srcMat.emissive_texname.empty())
            {
                auto imgResult = assets::load<image>(fs::view(contextPath + srcMat.emissive_texname));

                if (imgResult)
                    material.emissiveMap = imgResult.value();
                else
                {
                    warnings.push_back("Unable to load texture \"" + srcMat.emissive_texname + "\": " + imgResult.error().what());
                    material.emissiveMap = assets::invalid_asset<image>;
                }

                warnings.insert(warnings.end(), imgResult.warnings().begin(), imgResult.warnings().end());
            }

            if (!srcMat.normal_texname.empty())
            {
                auto imgResult = assets::load<image>(fs::view(contextPath + srcMat.normal_texname));

                if (imgResult)
                    material.normalMap = imgResult.value();
                else
                {
                    warnings.push_back("Unable to load texture \"" + srcMat.normal_texname + "\": " + imgResult.error().what());
                    material.normalMap = assets::invalid_asset<image>;
                }

                warnings.insert(warnings.end(), imgResult.warnings().begin(), imgResult.warnings().end());
            }

            material.aoMap = assets::invalid_asset<image>;

            if (!srcMat.bump_texname.empty())
            {
                auto imgResult = assets::load<image>(fs::view(contextPath + srcMat.bump_texname));

                if (imgResult)
                    material.heightMap = imgResult.value();
                else
                {
                    warnings.push_back("Unable to load texture \"" + srcMat.bump_texname + "\": " + imgResult.error().what());
                    material.heightMap = assets::invalid_asset<image>;
                }

                warnings.insert(warnings.end(), imgResult.warnings().begin(), imgResult.warnings().end());
            }

            if (progress)
                progress->advance_progress(percentagePerMat);
        }

        // Get all the vertex and composition data.
        tinyobj::attrib_t attributes = reader.GetAttrib();
        std::vector<tinyobj::shape_t> shapes = reader.GetShapes();

        // Sparse map like constructs to map both vertices and indices.
        std::vector<detail::vertex_hash> vertices;
        std::unordered_map<detail::vertex_hash, uint> indices;

        const float percentagePerShape = 10.f / static_cast<float>(shapes.size());

        // Iterate submeshes.
        for (auto& shape : shapes)
        {
            sub_mesh submesh;
            submesh.name = shape.name;
            submesh.indexOffset = data.indices.size();
            submesh.indexCount = shape.mesh.indices.size();
            if (shape.mesh.material_ids.size())
                submesh.materialIndex = shape.mesh.material_ids[0];

            const float percentagePerIndex = percentagePerShape / static_cast<float>(shape.mesh.indices.size());

            for (auto& indexData : shape.mesh.indices)
            {
                // Get the indices into the tinyobj attributes.

                const int vertexCount = static_cast<int>(attributes.vertices.size());
                const uint vertexIndex = static_cast<uint>((vertexCount + (indexData.vertex_index * 3)) % vertexCount);

                if (vertexIndex + 3 > attributes.vertices.size())
                    continue;

                const int normalCount = static_cast<int>(attributes.normals.size());
                const uint normalIndex = static_cast<uint>((normalCount + (indexData.normal_index * 3)) % normalCount);

                const int uvCount = static_cast<int>(attributes.texcoords.size());
                const uint uvIndex = static_cast<uint>((uvCount + (indexData.texcoord_index * 2)) % uvCount);

                // Extract the actual vertex data. (We flip the X axis to convert it to our left handed coordinate system.)
                math::vec3 vertex(-attributes.vertices[vertexIndex + 0], attributes.vertices[vertexIndex + 1], attributes.vertices[vertexIndex + 2]);

                math::color color = math::colors::white;
                if (vertexIndex + 2 < attributes.colors.size())
                    color = math::color(attributes.colors[vertexIndex + 0], attributes.colors[vertexIndex + 1], attributes.colors[vertexIndex + 2]);

                math::vec3 normal = math::vec3::up;
                if (normalIndex + 2 < attributes.normals.size())
                    normal = math::vec3(-attributes.normals[normalIndex + 0], attributes.normals[normalIndex + 1], attributes.normals[normalIndex + 2]);

                math::vec2 uv{};
                if (uvIndex + 1 < attributes.texcoords.size())
                    uv = math::vec2(attributes.texcoords[uvIndex + 0], attributes.texcoords[uvIndex + 1]);

                // Create a hash to check for doubles.
                detail::vertex_hash hash(vertex, color, normal, uv);

                // Use the properties of sparse containers to check for duplicate items.
                if (indices[hash] >= vertices.size() || vertices[indices[hash]] != hash)
                {
                    // Insert new hash into sparse container.
                    indices[hash] = static_cast<uint>(vertices.size());
                    vertices.push_back(hash);

                    // Append vertex data.
                    data.vertices.push_back(vertex);
                    data.colors.push_back(color);
                    data.normals.push_back(normal);
                    data.uvs.push_back(uv);
                }

                // Append the index of the newly added vertex or whichever one was added earlier.
                data.indices.push_back(indices[hash]);

                if (progress)
                    progress->advance_progress(percentagePerIndex);
            }

            // Add the sub-mesh to the mesh.
            data.submeshes.push_back(submesh);
        }

        // Because we only flip one axis we also need to flip the triangle rotation.
        for (size_type i = 0; i < data.indices.size(); i += 3)
        {
            uint i1 = data.indices[i + 1];
            uint i2 = data.indices[i + 2];
            data.indices[i + 1] = i2;
            data.indices[i + 2] = i1;
        }

        // Calculate the tangents.
        mesh::calculate_tangents(&data);

        // Construct and return the result.
        return { create(nameHash, data), warnings };
    }

    common::result<asset_ptr> ObjMeshLoader::load(id_type nameHash, const fs::view& file, const import_cfg& settings)
    {
        OPTICK_EVENT();
        return loadImpl(nameHash, file, settings, nullptr);
    }

    common::result<asset_ptr> ObjMeshLoader::loadAsync(id_type nameHash, const fs::view& file, const import_cfg& settings, progress_type& progress)
    {
        OPTICK_EVENT();
        return loadImpl(nameHash, file, settings, &progress);
    }
}
