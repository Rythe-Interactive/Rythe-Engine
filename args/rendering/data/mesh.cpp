#include <rendering/data/mesh.hpp>

#include <map>
#include <string>
#include <fstream>

namespace args::rendering
{
    void mesh_data::to_resource(fs::basic_resource* resource, const mesh_data& value)
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

    void mesh_data::from_resource(mesh_data* value, const fs::basic_resource& resource)
    {
        *value = mesh_data{};

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
            submesh_data submesh;
            retrieveBinaryData(submesh.name, start);
            retrieveBinaryData(submesh.indexCount, start);
            retrieveBinaryData(submesh.indexOffset, start);
            value->submeshes.push_back(submesh);
        }
    }

    void mesh_data::calculate_tangents(mesh_data* data)
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

    sparse_map<id_type, mesh> mesh_cache::m_meshes;
    async::readonly_rw_spinlock mesh_cache::m_meshLock;

    sparse_map<id_type, std::unique_ptr<mesh_data>> mesh_cache::m_meshdata;
    async::readonly_rw_spinlock mesh_cache::m_dataLock;

    bool mesh_handle::is_buffered()
    {
        return mesh_cache::get_mesh(id).buffered;
    }

    void mesh_handle::buffer_data(app::gl_id matrixBuffer)
    {
        mesh_cache::buffer(id, matrixBuffer);
    }

    inline const mesh_data& mesh_handle::get_data()
    {
        return mesh_cache::get_data(id);
    }

    inline const mesh& mesh_handle::get_mesh()
    {
        return mesh_cache::get_mesh(id);
    }

    inline const mesh& mesh_cache::get_mesh(id_type id)
    {
        async::readonly_guard guard(m_meshLock);
        return m_meshes[id];
    }

    inline const mesh_data& mesh_cache::get_data(id_type id)
    {
        async::readonly_guard guard(m_dataLock);
        return *m_meshdata[id];
    }

    void mesh_cache::buffer(id_type id, app::gl_id matrixBuffer)
    {
        mesh_data* data;

        {
            async::readonly_guard guard(m_dataLock);
            data = m_meshdata[id].get();
        }

        {
            async::readonly_guard guard(m_meshLock);
            mesh& mesh = m_meshes[id];

            glGenVertexArrays(1, &mesh.vertexArrayId);
            glBindVertexArray(mesh.vertexArrayId);

            glGenBuffers(1, &mesh.indexBufferId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indices.size() * sizeof(uint), data->indices.data(), GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            glGenBuffers(1, &mesh.vertexBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferId);
            glBufferData(GL_ARRAY_BUFFER, data->vertices.size() * sizeof(math::vec3), data->vertices.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(SV_POSITION);
            glVertexAttribPointer(SV_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glGenBuffers(1, &mesh.normalBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.normalBufferId);
            glBufferData(GL_ARRAY_BUFFER, data->normals.size() * sizeof(math::vec3), data->normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(SV_NORMAL);
            glVertexAttribPointer(SV_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glGenBuffers(1, &mesh.tangentBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.tangentBufferId);
            glBufferData(GL_ARRAY_BUFFER, data->tangents.size() * sizeof(math::vec3), data->tangents.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(SV_TANGENT);
            glVertexAttribPointer(SV_TANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glGenBuffers(1, &mesh.uvBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, mesh.uvBufferId);
            glBufferData(GL_ARRAY_BUFFER, data->uvs.size() * sizeof(math::vec2), data->uvs.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(SV_TEXCOORD0);
            glVertexAttribPointer(SV_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, 0);

            glBindBuffer(GL_ARRAY_BUFFER, matrixBuffer);
            glEnableVertexAttribArray(SV_MODELMATRIX + 0);
            glEnableVertexAttribArray(SV_MODELMATRIX + 1);
            glEnableVertexAttribArray(SV_MODELMATRIX + 2);
            glEnableVertexAttribArray(SV_MODELMATRIX + 3);

            glVertexAttribPointer(SV_MODELMATRIX + 0, 4, GL_FLOAT, GL_FALSE, sizeof(math::mat4), (GLvoid*)(0 * sizeof(math::mat4::col_type)));
            glVertexAttribPointer(SV_MODELMATRIX + 1, 4, GL_FLOAT, GL_FALSE, sizeof(math::mat4), (GLvoid*)(1 * sizeof(math::mat4::col_type)));
            glVertexAttribPointer(SV_MODELMATRIX + 2, 4, GL_FLOAT, GL_FALSE, sizeof(math::mat4), (GLvoid*)(2 * sizeof(math::mat4::col_type)));
            glVertexAttribPointer(SV_MODELMATRIX + 3, 4, GL_FLOAT, GL_FALSE, sizeof(math::mat4), (GLvoid*)(3 * sizeof(math::mat4::col_type)));

            glVertexAttribDivisor(SV_MODELMATRIX + 0, 1);
            glVertexAttribDivisor(SV_MODELMATRIX + 1, 1);
            glVertexAttribDivisor(SV_MODELMATRIX + 2, 1);
            glVertexAttribDivisor(SV_MODELMATRIX + 3, 1);

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            mesh.buffered = true;
        }
    }

    mesh_handle mesh_cache::create_mesh(const std::string& name, const fs::view& file, mesh_import_settings settings)
    {
        id_type id = nameHash(name);

        {
            async::readonly_guard guard(m_meshLock);
            if (m_meshes.contains(id))
                return { id };
        }

        if (!file.is_valid() || !file.file_info().is_file)
            return invalid_mesh_handle;

        auto result = fs::AssetImporter::tryLoad<mesh_data>(file, settings);

        if (result != common::valid)
            return invalid_mesh_handle;

        mesh_data* data;
        {
            async::readwrite_guard guard(m_dataLock);
            data = m_meshdata.emplace(id, new mesh_data(result)).first->get();
        }

        data->fileName = file.get_filename();

        mesh mesh{};

        for (auto& submeshData : data->submeshes)
            mesh.submeshes.push_back({ static_cast<uint>(submeshData.indexCount), static_cast<uint>(submeshData.indexOffset) });

        mesh.buffered = false;

        {
            async::readwrite_guard guard(m_meshLock);
            m_meshes.insert(id, mesh);
        }

        return { id };
    }

    inline mesh_handle mesh_cache::get_handle(const std::string& name)
    {
        id_type id = nameHash(name);
        async::readonly_guard guard(m_meshLock);
        if (m_meshes.contains(id))
            return { id };
        return invalid_mesh_handle;
    }

    inline mesh_handle mesh_cache::get_handle(id_type id)
    {
        async::readonly_guard guard(m_meshLock);
        if (m_meshes.contains(id))
            return { id };
        return invalid_mesh_handle;
    }
}
