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
#include <core/filesystem/basic_resolver.hpp>
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

    static image_handle loadGLTFImage(const tinygltf::Image& img)
    {
        auto handle = ImageCache::get_handle(img.name);
        if (handle)
            return handle;

        image image{};
        image.name = img.name;
        image.size.x = img.width;
        image.size.y = img.height;
        image.format = img.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ? channel_format::eight_bit : img.pixel_type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ? channel_format::sixteen_bit : channel_format::float_hdr;
        image.components = img.component == 1 ? image_components::grey : img.component == 2 ? image_components::grey_alpha : img.component == 3 ? image_components::rgb : image_components::rgba;
        image.dataSize = img.image.size();
        image.data = std::make_shared<byte_vec>(image.dataSize);

        memcpy(image.data->data(), img.image.data(), img.image.size());
        return ImageCache::insert_image(std::move(image));
    }

    /**
     * @brief Function to copy tinygltf buffer data into the correct mesh data vector
     *
     * @param buffer - The tinygltf::Buffer buffer containing the data
     * @param bufferView - the tinygltf::BufferView containing information about data size and offset
     * @param data - std::Vector<T> where the buffer is going to be copied into. The vector will be resized to vector.size()+(tinygltf data size)
     */
    template <class T>
    void handleGltfBuffer(const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView, std::vector<T>* data)
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
    static void handleGltfVertexColor(const tinygltf::Buffer& buffer, const tinygltf::BufferView& bufferView, int accessorType, int componentType, std::vector<math::color>* data)
    {
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
                log::warn("Vert colors for UNSIGNED BYTE not implemented");
            }
            else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                // Vertex colors in unsigned short
                // Currently not supported
                log::warn("Vert colors for UNSIGNED SHORT not implemented");
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
                log::warn("Vert colors were not stored as UNSIGNED BYTE/SHORT or float, skipping");
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
                log::warn("Vert colors for UNSIGNED BYTE not implemented");
            }
            else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            {
                // Vertex colors in unsigned short
                // Currently not supported
                log::warn("Vert colors for UNSIGNED SHORT not implemented");
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
                log::warn("Vert colors were not stored as UNSIGNED BYTE/SHORT or float, skipping");
            }
        }
        else log::warn("Vert colors were not vec3 or vec4, skipping colors");
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

    static void handleGltfMesh(mesh& meshData, const tinygltf::Model& model, const tinygltf::Mesh& mesh, const math::mat4& transform)
    {
        sub_mesh m;
        m.name = mesh.name;
        m.indexOffset = meshData.indices.size();

        if (mesh.primitives.size())
            m.materialIndex = mesh.primitives[0].material;

        for (auto primitive : mesh.primitives)
        {
            // Loop through all primitives in the mesh
            // Primitives can be vertex position, normal, texcoord (uv) and vertex colors

            if (primitive.indices < 0)
                continue;

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
                    detail::handleGltfVertexColor(buff, view, accessor.type, accessor.componentType, &(meshData.colors));
                }
                else
                {
                    log::warn("More data to be found in .gbl. Data can be accesed through: {}", attrib.first);
                }
            }

            size_type vertexCount = meshData.vertices.size() - vertexOffset;

            // Find the indices of our mesh and copy them into meshData.indices
            const tinygltf::Accessor& indexAccessor = model.accessors.at(static_cast<size_type>(primitive.indices));
            const tinygltf::BufferView& indexBufferView = model.bufferViews.at(static_cast<size_type>(indexAccessor.bufferView));
            const tinygltf::Buffer& indexBuffer = model.buffers.at(static_cast<size_type>(indexBufferView.buffer));
            detail::handleGltfIndices(indexBuffer, indexBufferView, vertexOffset, vertexCount, meshData.indices);
        }

        // Calculate size of submesh
        m.indexCount = meshData.indices.size() - m.indexOffset;
        meshData.submeshes.push_back(m);
    }

    static void handleGltfNode(mesh& meshData, const tinygltf::Model& model, const tinygltf::Node& node)
    {
        auto transf = detail::getGltfNodeTransform(node);

        if (node.mesh >= 0 && static_cast<size_type>(node.mesh) < model.meshes.size())
        {
            handleGltfMesh(meshData, model, model.meshes[static_cast<size_type>(node.mesh)], transf);
        }

        for (auto& nodeIdx : node.children)
        {
            if (static_cast<size_type>(nodeIdx) >= model.nodes.size())
            {
                log::warn("invalid node in GLTF");
                continue;
            }

            handleGltfNode(meshData, model, model.nodes[static_cast<size_type>(nodeIdx)]);
        }
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
    common::result<mesh, fs_error> obj_mesh_loader::load(const filesystem::basic_resource& resource, mesh_import_settings&& settings)
    {
        OPTICK_EVENT();

        // tinyobj objects
        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig config;

        // Configure settings.
        config.triangulate = settings.triangulate;
        config.vertex_color = true;

        std::string baseDir = "";

        filesystem::navigator navigator(settings.contextFolder.get_virtual_path());
        auto solution = navigator.find_solution();
        if (solution.has_error())
            log::warn(std::string("Invalid obj context path, ") + solution.error().what());
        else
        {
            auto s = solution.value();
            if (s.size() != 1)
                log::warn("Invalid obj context path, fs::view was not fully local");
            else
            {
                filesystem::basic_resolver* resolver = dynamic_cast<filesystem::basic_resolver*>(s[0].first);
                if (!resolver)
                    log::warn("Invalid obj context path, fs::view was not local");
                else
                {
                    resolver->set_target(s[0].second);

                    if (!resolver->is_valid_path())
                        log::warn("Invalid obj context path");
                    else
                        baseDir = resolver->get_absolute_path();
                }
            }
        }
        std::stringbuf obj_buf(resource.to_string());
        std::istream obj_ifs(&obj_buf);
        tinyobj::MaterialFileReader matFileReader(baseDir);

        // Try to parse the mesh data from the text data in the file.
        if (!reader.ParseFromString(resource.to_string(), matFileReader, config))
        {
            return legion_fs_error(reader.Error().c_str());
        }

        // Print any warnings.
        if (!reader.Warning().empty())
        {
            log::warn("Some warnings ocurred during loading of mesh set log filter to trace to see them.");
            std::string warnings = reader.Warning();
            common::replace_items(warnings, "\n", " ");
            log::trace(warnings.c_str());
        }

        // Create the mesh
        mesh data;

        std::vector<tinyobj::material_t> srcMaterials = reader.GetMaterials();

        for (auto& srcMat : srcMaterials)
        {
            auto& material = data.materials.emplace_back();
            material.name = srcMat.name;
            material.opaque = math::close_enough(srcMat.dissolve, 1);
            material.alphaCutoff = 0.5f;
            material.doubleSided = false;

            material.albedoValue = math::color(srcMat.diffuse[0], srcMat.diffuse[1], srcMat.diffuse[2]);
            if (!srcMat.diffuse_texname.empty())
                material.albedoMap = ImageCache::create_image(filesystem::view(settings.contextFolder.get_virtual_path() + srcMat.diffuse_texname));

            material.metallicValue = srcMat.metallic;
            if (!srcMat.metallic_texname.empty())
                material.metallicMap = ImageCache::create_image(filesystem::view(settings.contextFolder.get_virtual_path() + srcMat.metallic_texname));

            material.roughnessValue = srcMat.roughness;
            if (!srcMat.roughness_texname.empty())
                material.roughnessMap = ImageCache::create_image(filesystem::view(settings.contextFolder.get_virtual_path() + srcMat.roughness_texname));

            material.metallicRoughnessMap = invalid_image_handle;

            material.emissiveValue = math::color(srcMat.emission[0], srcMat.emission[1], srcMat.emission[2]);
            if (!srcMat.emissive_texname.empty())
                material.emissiveMap = ImageCache::create_image(filesystem::view(settings.contextFolder.get_virtual_path() + srcMat.emissive_texname));

            if (!srcMat.normal_texname.empty())
                material.normalMap = ImageCache::create_image(filesystem::view(settings.contextFolder.get_virtual_path() + srcMat.normal_texname));

            material.aoMap = invalid_image_handle;

            if (!srcMat.bump_texname.empty())
                material.heightMap = ImageCache::create_image(filesystem::view(settings.contextFolder.get_virtual_path() + srcMat.bump_texname));
        }

        // Get all the vertex and composition data.
        tinyobj::attrib_t attributes = reader.GetAttrib();
        std::vector<tinyobj::shape_t> shapes = reader.GetShapes();

        // Sparse map like constructs to map both vertices and indices.
        std::vector<detail::vertex_hash> vertices;
        std::unordered_map<detail::vertex_hash, uint> indices;

        // Iterate submeshes.
        for (auto& shape : shapes)
        {
            sub_mesh submesh;
            submesh.name = shape.name;
            submesh.indexOffset = data.indices.size();
            submesh.indexCount = shape.mesh.indices.size();
            if (shape.mesh.material_ids.size())
                submesh.materialIndex = shape.mesh.material_ids[0];

            for (auto& indexData : shape.mesh.indices)
            {
                // Get the indices into the tinyobj attributes.

                int vertexCount = static_cast<int>(attributes.vertices.size());
                uint vertexIndex = static_cast<uint>((vertexCount + (indexData.vertex_index * 3)) % vertexCount);

                if (vertexIndex + 3 > attributes.vertices.size())
                    continue;

                int normalCount = static_cast<int>(attributes.normals.size());
                uint normalIndex = static_cast<uint>((normalCount + (indexData.normal_index * 3)) % normalCount);

                int uvCount = static_cast<int>(attributes.texcoords.size());
                uint uvIndex = static_cast<uint>((uvCount + (indexData.texcoord_index * 2)) % uvCount);

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
        return std::move(data);
    }


    common::result<mesh, fs_error> gltf_binary_mesh_loader::load(const filesystem::basic_resource& resource, mesh_import_settings&& settings)
    {
        OPTICK_EVENT();

        namespace tg = tinygltf;

        tg::Model model;
        tg::TinyGLTF loader;
        std::string err;
        std::string warn;

        filesystem::navigator navigator(settings.contextFolder.get_virtual_path());
        auto solution = navigator.find_solution();
        if (solution.has_error())
        {
            log::warn(std::string("Invalid gltf context path, ") + solution.error().what());
        }

        auto s = solution.value();
        if (s.size() != 1)
        {
            log::warn("Invalid gltf context path, fs::view was not fully local");
        }

        filesystem::basic_resolver* resolver = dynamic_cast<filesystem::basic_resolver*>(s[0].first);
        if (!resolver)
        {
            log::warn("Invalid gltf context path, fs::view was not local");
        }

        resolver->set_target(s[0].second);

        if (!resolver->is_valid_path())
        {
            log::warn("Invalid gltf context path");
        }

        // Load gltf mesh data into model
        bool ret = loader.LoadBinaryFromMemory(&model, &err, &warn, resource.data(), static_cast<uint>(resource.size()), resolver->get_absolute_path());

        if (!err.empty())
        {
            // Check and print errors
            log::error("{}", err);
        }
        if (!warn.empty())
        {
            // Check and print warnings
            log::warn("{}", warn);
        }
        if (!ret)
        {
            // If the return failed, return error
            return legion_fs_error("Failed to parse GLTF");
        }

        core::mesh meshData;

        for (auto& srcMat : model.materials)
        {
            auto& material = meshData.materials.emplace_back();
            auto& pbrData = srcMat.pbrMetallicRoughness;

            material.name = srcMat.name;
            material.opaque = srcMat.alphaMode == "OPAQUE" || srcMat.alphaMode == "MASK";
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

            material.heightMap = invalid_image_handle;
        }

        if (model.scenes.size() <= 0)
        {
            return legion_fs_error("GLTF model contained 0 scenes");
        }

        size_type sceneToLoad = model.defaultScene > -1 ? static_cast<size_type>(model.defaultScene) : 0;

        for (auto& nodeIdx : model.scenes[sceneToLoad].nodes)
        {
            detail::handleGltfNode(meshData, model, model.nodes[static_cast<size_type>(nodeIdx)]);
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

        return std::move(meshData);
    }

    common::result<mesh, fs_error> gltf_ascii_mesh_loader::load(const filesystem::basic_resource& resource, mesh_import_settings&& settings)
    {
        namespace tg = tinygltf;

        tg::Model model;
        tg::TinyGLTF loader;
        std::string err;
        std::string warn;

        std::string ascii = resource.to_string();

        filesystem::navigator navigator(settings.contextFolder.get_virtual_path());
        auto solution = navigator.find_solution();
        if (solution.has_error())
        {
            log::warn(std::string("Invalid gltf context path, ") + solution.error().what());
        }

        auto s = solution.value();
        if (s.size() != 1)
        {
            log::warn("Invalid gltf context path, fs::view was not fully local");
        }

        filesystem::basic_resolver* resolver = dynamic_cast<filesystem::basic_resolver*>(s[0].first);
        if (!resolver)
        {
            log::warn("Invalid gltf context path, fs::view was not local");
        }

        resolver->set_target(s[0].second);

        if (!resolver->is_valid_path())
        {
            log::warn("Invalid gltf context path");
        }

        // Load gltf mesh data into model
        bool ret = loader.LoadASCIIFromString(&model, &err, &warn, ascii.c_str(), static_cast<uint>(ascii.length()), resolver->get_absolute_path());

        if (!err.empty())
        {
            // Check and print errors
            log::error("{}", err);
        }
        if (!warn.empty())
        {
            // Check and print warnings
            log::warn("{}", warn);
        }
        if (!ret)
        {
            // If the return failed, return error
            return legion_fs_error("Failed to parse glTF");
        }

        core::mesh meshData;

        for (auto& srcMat : model.materials)
        {
            auto& material = meshData.materials.emplace_back();
            auto& pbrData = srcMat.pbrMetallicRoughness;

            material.name = srcMat.name;
            material.opaque = srcMat.alphaMode == "OPAQUE" || srcMat.alphaMode == "MASK";
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

            material.heightMap = invalid_image_handle;
        }

        if (model.scenes.size() <= 0)
        {
            return legion_fs_error("GLTF model contained 0 scenes");
        }

        size_type sceneToLoad = model.defaultScene > -1 ? static_cast<size_type>(model.defaultScene) : 0;

        for (auto& nodeIdx : model.scenes[sceneToLoad].nodes)
        {
            detail::handleGltfNode(meshData, model, model.nodes[static_cast<size_type>(nodeIdx)]);
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

        return std::move(meshData);
    }
}
