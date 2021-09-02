#if !defined(DOXY_EXCLUDE)
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_USE_CPP14
#include <tinygltf/tiny_gltf.h> 
#endif

#include <core/data/loaders/gltfmeshloader.hpp>

namespace legion::core
{
    namespace detail
    {
        static assets::asset<image> loadGLTFImage(const tinygltf::Image& img)
        {
            auto hash = nameHash(img.name);

            auto handle = assets::get<image>(hash);
            if (handle)
                return handle;

            byte* imgData = new byte[img.image.size()]; // faux_gltf_image_loader will delete upon destruction.
            memcpy(imgData, img.image.data(), img.image.size());

            return assets::AssetCache<image>::createAsLoader<GltfFauxImageLoader>(hash, img.name, "",
                // Image constructor parameters.
                math::ivec2(img.width, img.height),
                img.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ? channel_format::eight_bit : img.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? channel_format::sixteen_bit : channel_format::float_hdr,
                img.component == 1 ? image_components::grey : img.component == 2 ? image_components::grey_alpha : img.component == 3 ? image_components::rgb : image_components::rgba,
                data_view<byte>{ imgData, img.image.size(), 0 });
        }

        /**
         * @brief Function to copy tinygltf buffer data into the correct mesh data vector
         *
         * @param buffer - The tinygltf::Buffer buffer containing the data
         * @param bufferView - the tinygltf::BufferView containing information about data size and offset
         * @param data - std::Vector<T> where the buffer is going to be copied into. The vector will be resized to vector.size()+(tinygltf data size)
         */
        template <class T>
        static void handleGltfBuffer(const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView, std::vector<T>* data)
        {
            size_t size = data->size();
            data->resize(size + bufferView.byteLength / sizeof(T));
            memcpy(data->data() + size, buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);
        }

        static void handleGltfBuffer(const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView, std::vector<math::vec3>* data, const math::mat4 transform, bool normal = false)
        {
            size_t size = data->size();
            data->resize(size + bufferView.byteLength / sizeof(math::vec3));
            memcpy(data->data() + size, buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);

            for (size_type i = size; i < data->size(); i++)
            {
                auto& item = data->at(i);
                if (normal)
                    item = math::normalize((transform * math::vec4(item.x, item.y, item.z, 0)).xyz());
                else
                    item = (transform * math::vec4(item.x, item.y, item.z, 1)).xyz();
            }
        }

        /**
         * @brief Function to handle vertex color of tinygltf
         *
         * @param buffer - tinygltf::Buffer containing the mesh data
         * @param bufferView - tinygltf::BufferView containing information about the buffer (data size/data offset)
         * @param accessorType - tinygltf accessorType, Vertex color is expected to come in vec3 or vec4 - will be handled by the function
         * @param componentType - tinygltf componentType, Vertex color is expected to come in float, unsigned byte or unsigned short - will be handled by the function
         * @param data - std::vector<color> the destination of the data copy. The vector will be resized to vector.size()+(tinygltf vertex color size)
         */
        static common::result<void, void> handleGltfVertexColor(const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView, int accessorType, int componentType, std::vector<math::color>* data)
        {
            std::vector<std::string> warnings;
            //colors in glft are in vec3/vec4 float/unsigned byte/unsigned short

            size_t size = data->size();
            if (accessorType == TINYGLTF_TYPE_VEC3)
            {
                // Copy the vertex colors into the vector, keeping in my mind that the vertex color data is only r,g,b

                size_t dataIndex = size;
                if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                {
                    // Vertex colors in unsigned byte
                    // Currently not supported
                    warnings.emplace_back("Vert colors for UNSIGNED BYTE not implemented");
                }
                else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    // Vertex colors in unsigned short
                    // Currently not supported
                    warnings.emplace_back("Vert colors for UNSIGNED SHORT not implemented");
                }
                else if (componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    // Vertex colors in float
                    data->resize(size + bufferView.byteLength / sizeof(math::vec3));
                    for (size_type i = 0; i < bufferView.byteLength; i += 3 * sizeof(float))
                    {
                        float r = *reinterpret_cast<const float*>(&buffer.data.at(i) + bufferView.byteOffset);
                        float g = *reinterpret_cast<const float*>(&buffer.data.at(i + sizeof(float)) + bufferView.byteOffset);
                        float b = *reinterpret_cast<const float*>(&buffer.data.at(i + 2 * sizeof(float)) + bufferView.byteOffset);
                        data->at(dataIndex++) = math::color(r, g, b);
                    }
                }
                else
                {
                    warnings.emplace_back("Vert colors were not stored as UNSIGNED BYTE/SHORT or float, skipping");
                }
            }
            else if (accessorType == TINYGLTF_TYPE_VEC4)
            {
                // Copy the vertex colors into the vector, keeping in my mind that the vertex color data is r,g,b,a

                size_t dataIndex = size;
                if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                {
                    // Vertex colors in unsigned byte
                    // Currently not supported
                    warnings.emplace_back("Vert colors for UNSIGNED BYTE not implemented");
                }
                else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    // Vertex colors in unsigned short
                    // Currently not supported
                    warnings.emplace_back("Vert colors for UNSIGNED SHORT not implemented");
                }
                else if (componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
                {
                    // Vertex colors in float
                    data->resize(size + bufferView.byteLength / sizeof(math::vec4));

                    for (size_type i = 0; i < bufferView.byteLength; i += 4 * sizeof(float))
                    {
                        float r = *reinterpret_cast<const float*>(&buffer.data.at(i) + bufferView.byteOffset);
                        float g = *reinterpret_cast<const float*>(&buffer.data.at(i + sizeof(float)) + bufferView.byteOffset);
                        float b = *reinterpret_cast<const float*>(&buffer.data.at(i + 2 * sizeof(float)) + bufferView.byteOffset);
                        float a = *reinterpret_cast<const float*>(&buffer.data.at(i + 3 * sizeof(float)) + bufferView.byteOffset);
                        data->at(dataIndex++) = math::color(r, g, b, a);
                    }
                }
                else
                {
                    warnings.emplace_back("Vert colors were not stored as UNSIGNED BYTE/SHORT or float, skipping");
                }
            }
            else warnings.emplace_back("Vert colors were not vec3 or vec4, skipping colors");

            return { common::success, warnings };
        }

        /**
         * @brief Function to copy tinygltf indices data into std::vector
         *
         * @param buffer - The tinygltf::Buffer containting mesh data
         * @param bufferView - the tinygltf::BufferView containting data about the buffer (data size/data offset)
         * @param offset - The mesh Indices offset. ( e.g. For the first submesh 0, for the second submesh submesh[0].indices.size() )
         * @param data - The std::vector to copy the indices data into
         */
        static void handleGltfIndices(const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView, size_type offset, size_type vertexCount, std::vector<uint>& data)
        {
            size_t size = data.size();
            //indices in glft are in int16
            data.reserve(size + bufferView.byteLength / sizeof(int16));

            std::vector<int16> origin;
            origin.resize(bufferView.byteLength / sizeof(int16));
            memcpy(origin.data(), buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);

            for (size_type i = 0; i < origin.size(); ++i)
                data.push_back(static_cast<uint>((static_cast<size_type>(static_cast<int>(vertexCount) + static_cast<int>(origin[i])) % vertexCount) + offset));
        }

        static math::mat4 getGltfNodeTransform(const tinygltf::Node& node)
        {
            if (node.matrix.size() == 16) {
                // Use `matrix' attribute
                return math::mat4(
                    static_cast<float>(node.matrix[0]), static_cast<float>(node.matrix[1]), static_cast<float>(node.matrix[2]), static_cast<float>(node.matrix[3]),
                    static_cast<float>(node.matrix[4]), static_cast<float>(node.matrix[5]), static_cast<float>(node.matrix[6]), static_cast<float>(node.matrix[7]),
                    static_cast<float>(node.matrix[8]), static_cast<float>(node.matrix[9]), static_cast<float>(node.matrix[10]), static_cast<float>(node.matrix[11]),
                    static_cast<float>(node.matrix[12]), static_cast<float>(node.matrix[13]), static_cast<float>(node.matrix[14]), static_cast<float>(node.matrix[15]));
            }
            else {
                math::vec3 pos{ 0,0,0 };
                math::quat rot{ 0,0,0,1 };
                math::vec3 scale{ 1,1,1 };

                // Assume Trans x Rotate x Scale order
                if (node.scale.size() == 3)
                    scale = math::vec3(static_cast<float>(node.scale[0]), static_cast<float>(node.scale[1]), static_cast<float>(node.scale[2]));

                if (node.rotation.size() == 4)
                    rot = math::quat(static_cast<float>(node.rotation[0]), static_cast<float>(node.rotation[1]), static_cast<float>(node.rotation[2]), static_cast<float>(node.rotation[3]));

                if (node.translation.size() == 3)
                    pos = math::vec3(static_cast<float>(node.translation[0]), static_cast<float>(node.translation[1]), static_cast<float>(node.translation[2]));

                return math::compose(scale, rot, pos);
            }
        }

        static common::result<void, void> handleGltfMesh(mesh& meshData, const tinygltf::Model& model, const tinygltf::Mesh& mesh, const math::mat4& transform)
        {
            std::vector<std::string> warnings;

            sub_mesh m;
            m.name = mesh.name;
            m.indexOffset = meshData.indices.size();

            if (mesh.primitives.size())
                m.materialIndex = mesh.primitives[0].material;

            for (auto primitive : mesh.primitives)
            {
                // Loop through all primitives in the mesh
                // Primitives can be vertex position, normal, texcoord (uv) and vertex colors

                size_type vertexOffset = meshData.vertices.size();

                for (auto& attrib : primitive.attributes)
                {
                    // Loop through the attributes of the primitive
                    // Depending on the attribute the data is copied into a different std::vector in meshData

                    const tinygltf::Accessor& accessor = model.accessors.at(static_cast<size_type>(attrib.second));
                    const tinygltf::BufferView& view = model.bufferViews.at(static_cast<size_type>(accessor.bufferView));
                    const tinygltf::Buffer& buff = model.buffers.at(static_cast<size_type>(view.buffer));
                    if (attrib.first.compare("POSITION") == 0)
                    {
                        // Position data
                        detail::handleGltfBuffer(buff, view, &(meshData.vertices), transform);
                    }
                    else if (attrib.first.compare("NORMAL") == 0)
                    {
                        // Normal data
                        detail::handleGltfBuffer(buff, view, &(meshData.normals), transform, true);
                    }
                    else if (attrib.first.compare("TEXCOORD_0") == 0)
                    {
                        // UV data
                        detail::handleGltfBuffer(buff, view, &(meshData.uvs));
                    }
                    else if (attrib.first.compare("COLOR_0") == 0)
                    {
                        // Vertex color data
                        auto result = detail::handleGltfVertexColor(buff, view, accessor.type, accessor.componentType, &(meshData.colors));
                        warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());
                    }
                    else
                    {
                        log::warn("More data to be found in gltf. Data can be accesed through: {}", attrib.first);
                    }
                }

                size_type vertexCount = meshData.vertices.size() - vertexOffset;

                // Find the indices of our mesh and copy them into meshData.indices
                size_type index = primitive.indices < 0 ? vertexCount - static_cast<size_type>(primitive.indices * -1) : static_cast<size_type>(primitive.indices);
                const tinygltf::Accessor& indexAccessor = model.accessors.at(index);
                const tinygltf::BufferView& indexBufferView = model.bufferViews.at(static_cast<size_type>(indexAccessor.bufferView));
                const tinygltf::Buffer& indexBuffer = model.buffers.at(static_cast<size_type>(indexBufferView.buffer));
                detail::handleGltfIndices(indexBuffer, indexBufferView, vertexOffset, vertexCount, meshData.indices);
            }

            // Calculate size of submesh
            m.indexCount = meshData.indices.size() - m.indexOffset;
            meshData.submeshes.push_back(m);

            return { common::success, warnings };
        }

        static common::result<void, void> handleGltfNode(mesh& meshData, const tinygltf::Model& model, const tinygltf::Node& node)
        {
            std::vector<std::string> warnings;
            auto transf = detail::getGltfNodeTransform(node);

            if (static_cast<size_type>(node.mesh) < model.meshes.size())
            {
                size_type meshIdx = node.mesh < 0 ? model.meshes.size() - static_cast<size_type>(node.mesh * -1) : static_cast<size_type>(node.mesh);
                auto result = handleGltfMesh(meshData, model, model.meshes[meshIdx], transf);
                warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());
            }

            for (auto& nodeIdx : node.children)
            {
                size_type idx;
                if (static_cast<size_type>(nodeIdx) >= model.nodes.size())
                {
                    warnings.emplace_back("invalid node in GLTF");
                    continue;
                }
                else if (nodeIdx < 0)
                    idx = model.nodes.size() - static_cast<size_type>(nodeIdx * -1);
                else
                    idx = static_cast<size_type>(nodeIdx);

                auto result = handleGltfNode(meshData, model, model.nodes[idx]);
                warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());
            }

            return { common::success, warnings };
        }
    }

    using base = assets::AssetLoader<mesh>;
    using asset_ptr = base::asset_ptr;
    using import_cfg = base::import_cfg;
    using progress_type = base::progress_type;

    common::result<asset_ptr> GltfMeshLoader::loadImpl(id_type nameHash, const fs::view& file, progress_type* progress)
    {
        namespace tg = tinygltf;

        tg::Model model;
        tg::TinyGLTF loader;
        std::string err;
        std::string warn;

        std::string baseDir = "";
        std::vector<std::string> warnings;
        auto contextPath = file.parent().get_virtual_path();

        filesystem::navigator navigator(contextPath);
        auto solution = navigator.find_solution();
        if (solution.has_error())
            warnings.push_back(std::string("Invalid gltf context path, ") + solution.error().what());
        else
        {
            auto s = solution.value();
            if (s.size() != 1)
                warnings.emplace_back("Invalid gltf context path, fs::view was not fully local");
            else
            {
                filesystem::basic_resolver* resolver = dynamic_cast<filesystem::basic_resolver*>(s[0].first);
                if (!resolver)
                    warnings.emplace_back("Invalid gltf context path, fs::view was not local");
                else
                {
                    resolver->set_target(s[0].second);

                    if (!resolver->is_valid_path())
                        warnings.emplace_back("Invalid gltf context path");
                    else
                        baseDir = resolver->get_absolute_path();
                }
            }
        }

        auto result = file.get();

        warnings.insert(warnings.end(), result.warnings().begin(), result.warnings().end());
        if (!result)
            return { legion_exception_msg(result.error().what()), warnings };

        auto extension = file.get_extension();
        if (!extension)
            return { legion_exception_msg(extension.error().what()), warnings };


        if (progress)
            progress->advance_progress(5.f);

        if (extension.value() == ".gltf")
        {
            auto text = result->to_string();
            if (!loader.LoadASCIIFromString(&model, &err, &warn, text.c_str(), static_cast<uint>(text.size()), baseDir))
            {
                auto parserWarnings = common::split_string_at<'\n'>(warn);
                warnings.insert(warnings.end(), parserWarnings.begin(), parserWarnings.end());
                return { legion_exception_msg("Failed to parse GLTF: " + err), warnings };
            }
        }
        else if (extension.value() == ".glb")
        {
            if (!loader.LoadBinaryFromMemory(&model, &err, &warn, result->data(), static_cast<uint>(result->size()), baseDir))
            {
                auto parserWarnings = common::split_string_at<'\n'>(warn);
                warnings.insert(warnings.end(), parserWarnings.begin(), parserWarnings.end());
                return { legion_exception_msg("Failed to parse GLTF: " + err), warnings };
            }
        }
        else
            return { legion_exception_msg("File was not recognised as a gltf file."), warnings };

        if (progress)
            progress->advance_progress(55.f);

        if (!err.empty())
        {
            auto parserErrors = common::split_string_at<'\n'>(err);
            warnings.insert(warnings.end(), parserErrors.begin(), parserErrors.end());
        }
        if (!warn.empty())
        {
            auto parserWarnings = common::split_string_at<'\n'>(warn);
            warnings.insert(warnings.end(), parserWarnings.begin(), parserWarnings.end());
        }

        core::mesh meshData;

        const float percentagePerMat = 25.f / static_cast<float>(model.materials.size());

        for (auto& srcMat : model.materials)
        {
            auto& material = meshData.materials.emplace_back();
            auto& pbrData = srcMat.pbrMetallicRoughness;

            material.name = srcMat.name;
            material.opaque = srcMat.alphaMode != "BLEND";
            material.alphaCutoff = static_cast<float>(srcMat.alphaCutoff);
            material.doubleSided = srcMat.doubleSided;

            material.albedoValue = math::color(
                static_cast<float>(pbrData.baseColorFactor[0]),
                static_cast<float>(pbrData.baseColorFactor[1]),
                static_cast<float>(pbrData.baseColorFactor[2]),
                static_cast<float>(pbrData.baseColorFactor[3]));

            if (pbrData.baseColorTexture.index >= 0)
                material.albedoMap = detail::loadGLTFImage(
                    model.images[
                        static_cast<size_type>(model.textures[
                            static_cast<size_type>(pbrData.baseColorTexture.index)
                        ].source)
                    ]);

            material.metallicValue = static_cast<float>(pbrData.metallicFactor);
            material.roughnessValue = static_cast<float>(pbrData.roughnessFactor);

            if (pbrData.metallicRoughnessTexture.index >= 0)
                material.metallicRoughnessMap = detail::loadGLTFImage(
                    model.images[
                        static_cast<size_type>(model.textures[
                            static_cast<size_type>(pbrData.metallicRoughnessTexture.index)
                        ].source)
                    ]);

            material.emissiveValue = math::color(
                static_cast<float>(srcMat.emissiveFactor[0]),
                static_cast<float>(srcMat.emissiveFactor[1]),
                static_cast<float>(srcMat.emissiveFactor[2]));

            if (srcMat.emissiveTexture.index >= 0)
                material.emissiveMap = detail::loadGLTFImage(
                    model.images[
                        static_cast<size_type>(model.textures[
                            static_cast<size_type>(srcMat.emissiveTexture.index)
                        ].source)
                    ]);

            if (srcMat.normalTexture.index >= 0)
                material.normalMap = detail::loadGLTFImage(
                    model.images[
                        static_cast<size_type>(model.textures[
                            static_cast<size_type>(srcMat.normalTexture.index)
                        ].source)
                    ]);

            if (srcMat.occlusionTexture.index >= 0)
                material.aoMap = detail::loadGLTFImage(
                    model.images[
                        static_cast<size_type>(model.textures[
                            static_cast<size_type>(srcMat.occlusionTexture.index)
                        ].source)
                    ]);

            material.heightMap = assets::invalid_asset<image>;

            if (progress)
                progress->advance_progress(percentagePerMat);
        }

        if (!model.scenes.size())
        {
            return { legion_exception_msg("GLTF model contained 0 scenes"), warnings };
        }

        const size_type sceneToLoad = model.defaultScene < 0 ? model.scenes.size() - static_cast<size_type>(model.defaultScene * -1) : static_cast<size_type>(model.defaultScene);

        const float percentagePerNode = 10.f / static_cast<float>(model.scenes[sceneToLoad].nodes.size());

        for (auto& nodeIdx : model.scenes[sceneToLoad].nodes)
        {
            const size_type idx = nodeIdx < 0 ? model.nodes.size() - static_cast<size_type>(nodeIdx * -1) : static_cast<size_type>(nodeIdx);
            detail::handleGltfNode(meshData, model, model.nodes[idx]);

            if (progress)
                progress->advance_progress(percentagePerNode);
        }

        // Convert to left handed coord system
        for (size_type i = 0; i < meshData.vertices.size(); ++i)
        {
            meshData.vertices[i] = meshData.vertices[i] * math::vec3(-1, 1, 1);

            if (meshData.normals.size() == i)
                meshData.normals.push_back(math::vec3::up);
            else
                meshData.normals[i] = meshData.normals[i] * math::vec3(-1, 1, 1);

            if (meshData.uvs.size() == i)
                meshData.uvs.push_back(math::vec2(0, 0));
            else
                meshData.uvs[i] = meshData.uvs[i] * math::vec2(1, -1);

            if (meshData.colors.size() == i)
                meshData.colors.push_back(core::math::colors::grey);
        }

        // Because we only flip one axis we also need to flip the triangle rotation.
        for (size_type i = 0; i < meshData.indices.size(); i += 3)
        {
            uint i1 = meshData.indices[i + 1];
            uint i2 = meshData.indices[i + 2];
            meshData.indices[i + 1] = i2;
            meshData.indices[i + 2] = i1;
        }

        mesh::calculate_tangents(&meshData);

        return { create(nameHash, meshData), warnings };
    }

    bool GltfMeshLoader::canLoad(const fs::view& file)
    {
        auto result = file.get_extension();
        if (!result)
            return false;

        return result.value() == ".gltf" || result.value() == ".glb";
    }

    common::result<asset_ptr> GltfMeshLoader::load(id_type nameHash, const fs::view& file, L_MAYBEUNUSED const import_cfg& settings)
    {
        OPTICK_EVENT();
        return loadImpl(nameHash, file, nullptr);
    }

    common::result<asset_ptr> GltfMeshLoader::loadAsync(id_type nameHash, const fs::view& file, L_MAYBEUNUSED const import_cfg& settings, progress_type& progress)
    {
        OPTICK_EVENT();
        return loadImpl(nameHash, file, &progress);
    }
}
