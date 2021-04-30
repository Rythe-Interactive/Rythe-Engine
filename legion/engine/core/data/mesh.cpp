#include <core/data/mesh.hpp>
#include <core/data/importers/mesh_importers.hpp>

namespace legion::core
{
    std::unordered_map<id_type, std::unique_ptr<std::pair<async::rw_spinlock, mesh>>> MeshCache::m_meshes;
    async::rw_spinlock MeshCache::m_meshesLock;
    id_type MeshCache::debugId;

    void mesh::to_resource(filesystem::basic_resource* resource, const mesh& value)
    {
        OPTICK_EVENT();
        // Erase all previous data.
        resource->clear();

        // Write new data.
        auto& data = resource->get();
        appendBinaryData(&value.filePath, data);
        appendBinaryData(&value.vertices, data);
        appendBinaryData(&value.colors, data);
        appendBinaryData(&value.normals, data);
        appendBinaryData(&value.uvs, data);
        appendBinaryData(&value.tangents, data);
        appendBinaryData(&value.indices, data);

        // Append each submesh data.
        uint64 submeshCount = value.submeshes.size();
        appendBinaryData(&submeshCount, data);

        for (auto& submesh : value.submeshes)
        {
            appendBinaryData(&submesh.name, data);
            appendBinaryData(&submesh.indexCount, data);
            appendBinaryData(&submesh.indexOffset, data);
        }
    }

    void mesh::from_resource(mesh* value, const filesystem::basic_resource& resource)
    {
        OPTICK_EVENT();
        *value = mesh{};

        // Get point from which to start reading.
        byte_vec::const_iterator start = resource.begin();

        // Read data
        retrieveBinaryData(value->filePath, start);
        retrieveBinaryData(value->vertices, start);
        retrieveBinaryData(value->colors, start);
        retrieveBinaryData(value->normals, start);
        retrieveBinaryData(value->uvs, start);
        retrieveBinaryData(value->tangents, start);
        retrieveBinaryData(value->indices, start);

        // Read sub-mesh count
        uint64 submeshCount;
        retrieveBinaryData(submeshCount, start);

        // Read and append all sub-meshes
        for (size_type i = 0; i < submeshCount; i++)
        {
            sub_mesh submesh;
            retrieveBinaryData(submesh.name, start);
            retrieveBinaryData(submesh.indexCount, start);
            retrieveBinaryData(submesh.indexOffset, start);
            value->submeshes.push_back(submesh);
        }
    }

    void mesh::calculate_tangents(mesh* data)
    {
        OPTICK_EVENT();
        // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
        data->tangents.resize(data->normals.size());

        // Iterate over all triangles of each sub-mesh.
        for (auto& submesh : data->submeshes)
            for (size_type i = submesh.indexOffset; i < submesh.indexOffset + submesh.indexCount; i += 3)
            {
                if (i + 2 > data->indices.size())
                    break;

                math::uvec3 indices{ data->indices[i], data->indices[i + 1], data->indices[i + 2] };

                if (indices[0] > data->vertices.size() || indices[1] > data->vertices.size() || indices[2] > data->vertices.size())
                    continue;

                // Get vertices of the triangle.
                math::vec3 vtx0 = data->vertices[indices[0]];
                math::vec3 vtx1 = data->vertices[indices[1]];
                math::vec3 vtx2 = data->vertices[indices[2]];

                // Get UVs of the triangle.
                math::vec2 uv0 = data->uvs[indices[0]];
                math::vec2 uv1 = data->uvs[indices[1]];
                math::vec2 uv2 = data->uvs[indices[2]];

                // Get primary edges
                math::vec3 edge0 = vtx1 - vtx0;
                math::vec3 edge1 = vtx2 - vtx0;

                // Get difference in uv over the two primary edges.
                math::vec2 deltaUV0 = uv1 - uv0;
                math::vec2 deltaUV1 = uv2 - uv0;

                // Get inverse of the determinant of the UV tangent matrix.
                float inverseUVDeterminant = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

                // T = tangent
                // B = bi-tangent
                // E0 = first primary edge
                // E1 = second primary edge
                // dU0 = delta of x texture coordinates of the first primary edge
                // dV0 = delta of y texture coordinates of the first primary edge
                // dU1 = delta of x texture coordinates of the second primary edge
                // dV1 = delta of y texture coordinates of the second primary edge
                // ┌          ┐          1        ┌           ┐ ┌             ┐
                // │ Tx Ty Tz │ _ ─────────────── │  dV1 -dV0 │ │ E0x E0y E0z │
                // │ Bx By Bz │ ─ dU0ΔV1 - dU1ΔV0 │ -dU1  dU0 │ │ E1x E1y E1z │
                // └          ┘                   └           ┘ └             ┘
                math::vec3 tangent;
                tangent.x = inverseUVDeterminant * ((deltaUV1.y * edge0.x) - (deltaUV0.y * edge1.x));
                tangent.y = inverseUVDeterminant * ((deltaUV1.y * edge0.y) - (deltaUV0.y * edge1.y));
                tangent.z = inverseUVDeterminant * ((deltaUV1.y * edge0.z) - (deltaUV0.y * edge1.z));

                // Check if the tangent is valid.
                if (tangent == math::vec3(0, 0, 0) || tangent != tangent)
                    continue;

                // Normalize the tangent.
                tangent = math::normalize(tangent);

                // Accumulate the tangent in order to be able to smooth it later.
                data->tangents[indices[0]] += tangent;
                data->tangents[indices[1]] += tangent;
                data->tangents[indices[2]] += tangent;
            }

        // Smooth all tangents.
        for (size_type i = 0; i < data->tangents.size(); i++)
            if (data->tangents[i] != math::vec3::zero)
                data->tangents[i] = math::normalize(data->tangents[i]);
    }

    std::pair<async::rw_spinlock&, mesh&> mesh_handle::get()
    {
        OPTICK_EVENT();
        async::readonly_guard guard(MeshCache::m_meshesLock);
        auto& [lock, mesh] = *(MeshCache::m_meshes[id].get());
        return std::make_pair(std::ref(lock), std::ref(mesh));
    }

    mesh_handle MeshCache::create_mesh(const std::string& name, const filesystem::view& file, mesh_import_settings settings)
    {
        OPTICK_EVENT();
        // Get ID.
        id_type id = nameHash(name);

        { // Check if the mesh already exists, and return that instead if it does.
            async::readonly_guard guard(m_meshesLock);
            if (m_meshes.count(id))
                return { id };
        }

        if (!file.is_valid() || !file.file_info().is_file)
            return invalid_mesh_handle;

        // Try to load the mesh.
        auto result = filesystem::AssetImporter::tryLoad<mesh>(file, settings);

        if (result != common::valid)
        {
            log::error("Error while loading file: {} {}", static_cast<std::string>(file.get_filename()), result.get_error());
            return invalid_mesh_handle;
        }

        mesh data = result;
        data.filePath = file.get_virtual_path(); // Set the filename.

        { // Insert the mesh into the mesh list.
            async::readwrite_guard guard(m_meshesLock);
            auto* pair_ptr = new std::pair<async::rw_spinlock, mesh>();
            pair_ptr->second = std::move(data);
            m_meshes.emplace(id, std::unique_ptr<std::pair<async::rw_spinlock, mesh>>(pair_ptr));
        }

        return { id };
    }

    mesh_handle MeshCache::create_mesh(const std::string& name, const mesh& meshData)
    {
        id_type newId = nameHash(name); // Get the new id.

        async::readwrite_guard guard(m_meshesLock);
        auto* pair_ptr = new std::pair<async::rw_spinlock, mesh>();
        pair_ptr->second = std::move(meshData);
        m_meshes.emplace(newId, std::unique_ptr<std::pair<async::rw_spinlock, mesh>>(pair_ptr));

        return { newId };
    }

    mesh_handle MeshCache::copy_mesh(const std::string& name, const std::string& newName)
    {
        OPTICK_EVENT();
        id_type id = nameHash(name); // Get the id of the original mesh.
        id_type newId = nameHash(newName); // Get the new id.

        {
            async::readonly_guard guard(m_meshesLock);
            mesh data = m_meshes[id]->second; // Get a copy of the original mesh.

            if (m_meshes.count(newId))
            {// If the new mesh already exists, overwrite it.
                mesh& destination = m_meshes[newId]->second;
                async::readwrite_guard rwguard(m_meshes[newId]->first);
                destination = data;
            }
            else // If the new mesh doesn't exist yet create it with the copy.
            {
                auto* pair_ptr = new std::pair<async::rw_spinlock, mesh>();
                pair_ptr->second = data;
                m_meshes.emplace(std::make_pair(newId, pair_ptr));
            }
        }
        return { newId }; // Return a handle to the new mesh.
    }

    mesh_handle MeshCache::copy_mesh(id_type id, const std::string& newName)
    {
        OPTICK_EVENT();
        id_type newId = nameHash(newName); // Get the new id.

        {
            async::readonly_guard guard(m_meshesLock);
            mesh data = m_meshes[id]->second; // Get a copy of the original mesh.

            if (m_meshes.count(newId))
            {// If the new mesh already exists, overwrite it.
                mesh& destination = m_meshes[newId]->second;
                async::readwrite_guard rwguard(m_meshes[newId]->first);
                destination = data;
            }
            else // If the new mesh doesn't exist yet create it with the copy.
            {
                auto* pair_ptr = new std::pair<async::rw_spinlock, mesh>();
                pair_ptr->second = data;
                m_meshes.emplace(std::make_pair(newId, pair_ptr));
            }
        }
        return { newId }; // Return a handle to the new mesh.
    }

    mesh_handle MeshCache::get_handle(const std::string& name)
    {
        OPTICK_EVENT();
        id_type id = nameHash(name);
        async::readonly_guard guard(MeshCache::m_meshesLock);
        if (MeshCache::m_meshes.count(id))
            return { id };
        return invalid_mesh_handle;
    }

    mesh_handle MeshCache::get_handle(id_type id)
    {
        OPTICK_EVENT();
        async::readonly_guard guard(MeshCache::m_meshesLock);
        if (MeshCache::m_meshes.count(id))
            return { id };
        return invalid_mesh_handle;
    }

    void MeshCache::destroy_mesh(id_type id)
    {
        bool erased = false;

        {
            async::readwrite_guard guard(MeshCache::m_meshesLock);

            if (!m_meshes.count(id))
                return;

            erased = m_meshes.erase(id);
        }

        if (erased)
            log::debug("Destroyed mesh {}", id);
    }
}
