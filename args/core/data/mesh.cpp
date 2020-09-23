#include <core/data/mesh.hpp>
#include <core/data/importers/mesh_importers.hpp>

namespace args::core
{
    std::unordered_map<id_type, std::unique_ptr<std::pair<async::readonly_rw_spinlock, mesh>>> MeshCache::m_meshes;
    async::readonly_rw_spinlock MeshCache::m_meshesLock;

    void mesh::to_resource(filesystem::basic_resource* resource, const mesh& value)
    {
        resource->clear();
        appendBinaryData(&value.fileName, resource->get());
        appendBinaryData(&value.vertices, resource->get());
        appendBinaryData(&value.normals, resource->get());
        appendBinaryData(&value.uvs, resource->get());
        appendBinaryData(&value.tangents, resource->get());
        appendBinaryData(&value.indices, resource->get());

        uint64 submeshCount = value.submeshes.size();
        appendBinaryData(&submeshCount, resource->get());

        for (auto& submesh : value.submeshes)
        {
            appendBinaryData(&submesh.name, resource->get());
            appendBinaryData(&submesh.indexCount, resource->get());
            appendBinaryData(&submesh.indexOffset, resource->get());
        }
    }

    void mesh::from_resource(mesh* value, const filesystem::basic_resource& resource)
    {
        *value = mesh{};

        byte_vec::const_iterator start = resource.begin();

        retrieveBinaryData(value->fileName, start);
        retrieveBinaryData(value->vertices, start);
        retrieveBinaryData(value->normals, start);
        retrieveBinaryData(value->uvs, start);
        retrieveBinaryData(value->tangents, start);
        retrieveBinaryData(value->indices, start);

        uint64 submeshCount;

        retrieveBinaryData(submeshCount, start);

        for (int i = 0; i < submeshCount; i++)
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
        data->tangents.resize(data->normals.size());

        for (auto& submesh : data->submeshes)
            for (unsigned i = submesh.indexOffset; i < submesh.indexOffset + submesh.indexCount; i += 3)
            {
                math::vec3 vtx0 = data->vertices[data->indices[i]];
                math::vec3 vtx1 = data->vertices[data->indices[i + 1]];
                math::vec3 vtx2 = data->vertices[data->indices[i + 2]];

                math::vec2 uv0 = data->uvs[data->indices[i]];
                math::vec2 uv2 = data->uvs[data->indices[i + 2]];
                math::vec2 uv1 = data->uvs[data->indices[i + 1]];

                math::vec3 edge0 = vtx1 - vtx0;
                math::vec3 edge1 = vtx2 - vtx0;

                math::vec2 deltaUV0 = uv1 - uv0;
                math::vec2 deltaUV1 = uv2 - uv0;

                float uvDetFrac = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

                math::vec3 tangent;
                tangent.x = uvDetFrac * ((deltaUV1.y * edge0.x) - (deltaUV0.y * edge1.x));
                tangent.y = uvDetFrac * ((deltaUV1.y * edge0.y) - (deltaUV0.y * edge1.y));
                tangent.z = uvDetFrac * ((deltaUV1.y * edge0.z) - (deltaUV0.y * edge1.z));
                if (tangent == math::vec3(0, 0, 0) || tangent != tangent)
                    continue;

                tangent = math::normalize(tangent);

                data->tangents[data->indices[i]] += tangent;
                data->tangents[data->indices[i + 1]] += tangent;
                data->tangents[data->indices[i + 2]] += tangent;
            }

        for (unsigned i = 0; i < data->tangents.size(); i++)
            if (data->tangents[i] != math::vec3(0, 0, 0))
                data->tangents[i] = math::normalize(data->tangents[i]);
    }

    std::pair<async::readonly_rw_spinlock&, mesh&> mesh_handle::get()
    {
        async::readonly_guard guard(MeshCache::m_meshesLock);
        auto& [lock, mesh] = *(MeshCache::m_meshes[id].get());
        return std::make_pair(std::ref(lock), std::ref(mesh));
    }

    mesh_handle MeshCache::create_mesh(const std::string& name, const filesystem::view& file, mesh_import_settings settings)
    {
        id_type id = nameHash(name);

        {
            async::readonly_guard guard(m_meshesLock);
            if (m_meshes.count(id))
                return { id };
        }

        if (!file.is_valid() || !file.file_info().is_file)
            return invalid_mesh_handle;

        auto result = filesystem::AssetImporter::tryLoad<mesh>(file, settings);

        if (result != common::valid)
            return invalid_mesh_handle;

        mesh* data;

        {            
            async::readwrite_guard guard(m_meshesLock);
            auto* pair_ptr = new std::pair<async::readonly_rw_spinlock, mesh>(std::make_pair<async::readonly_rw_spinlock, mesh>(async::readonly_rw_spinlock(), result));
            auto iterator = m_meshes.emplace(std::make_pair(id, std::unique_ptr<std::pair<async::readonly_rw_spinlock, mesh>>(pair_ptr))).first;
            data = &iterator->second.get()->second;
        }

        data->fileName = file.get_filename();

        return { id };
    }

    mesh_handle MeshCache::copy_mesh(const std::string& name, const std::string& newName)
    {
        id_type id = nameHash(name);
        id_type newId = nameHash(newName);            

        {
            async::readonly_guard guard(m_meshesLock);
            mesh data = m_meshes[id]->second;

            if (m_meshes.count(newId))
            {
                mesh& destination = m_meshes[newId]->second;
                async::readwrite_guard rwguard(m_meshes[newId]->first);
                destination = data;
            }
            else
            {
                auto* pair_ptr = new std::pair<async::readonly_rw_spinlock, mesh>(std::make_pair(async::readonly_rw_spinlock(), data));
                m_meshes.emplace(std::make_pair(newId, pair_ptr));
            }
        }
        return { newId };
    }

    mesh_handle MeshCache::copy_mesh(id_type id, const std::string& newName)
    {
        id_type newId = nameHash(newName);

        if (m_meshes.count(newId))
            m_meshes.erase(newId);

        mesh data = m_meshes[id]->second;

        auto* pair_ptr = new std::pair<async::readonly_rw_spinlock, mesh>(std::make_pair(async::readonly_rw_spinlock(), data));
        m_meshes.emplace(std::make_pair(newId, pair_ptr));
        return { newId };
    }

    mesh_handle MeshCache::get_handle(const std::string& name)
    {
        id_type id = nameHash(name);
        async::readonly_guard guard(MeshCache::m_meshesLock);
        if (MeshCache::m_meshes.count(id))
            return { id };
        return invalid_mesh_handle;
    }

    mesh_handle MeshCache::get_handle(id_type id)
    {
        async::readonly_guard guard(MeshCache::m_meshesLock);
        if (MeshCache::m_meshes.count(id))
            return { id };
        return invalid_mesh_handle;
    }
}
