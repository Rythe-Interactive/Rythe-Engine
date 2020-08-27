#include <rendering/data/mesh.hpp>

#include <map>
#include <string>
#include <fstream>

namespace args::rendering
{
    sparse_map<id_type, mesh> mesh_cache::m_meshes;
    async::readonly_rw_spinlock mesh_cache::m_meshLock;

    sparse_map<id_type, std::unique_ptr<mesh_data>> mesh_cache::m_meshdata;
    async::readonly_rw_spinlock mesh_cache::m_dataLock;

    mesh_data* mesh_handle::get_data()
    {
        return nullptr;
    }

    inline const mesh& mesh_cache::get_mesh(id_type id)
    {
        async::readonly_guard guard(m_meshLock);
        return m_meshes[id];
    }

    void mesh_cache::buffer(mesh_data* data, mesh& mesh)
    {
        for (int i = 0; i < mesh.submeshes.size(); i++)
        {
            auto& submesh = mesh.submeshes[i];
            auto& submeshData = data->submeshes[i];
            submesh.indexCount = submeshData.indices.size();

            glGenBuffers(1, &submesh.indexBufferId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh.indexBufferId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, submeshData.indices.size() * sizeof(unsigned int), &submeshData.indices[0], GL_STATIC_DRAW);
        }

        glGenBuffers(1, &mesh.vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, data->vertices.size() * sizeof(math::vec3), &data->vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &mesh.normalBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.normalBufferId);
        glBufferData(GL_ARRAY_BUFFER, data->normals.size() * sizeof(math::vec3), &data->normals[0], GL_STATIC_DRAW);

        glGenBuffers(1, &mesh.uvBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.uvBufferId);
        glBufferData(GL_ARRAY_BUFFER, data->uvs.size() * sizeof(math::vec2), &data->uvs[0], GL_STATIC_DRAW);

        glGenBuffers(1, &mesh.tangentBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.tangentBufferId);
        glBufferData(GL_ARRAY_BUFFER, data->tangents.size() * sizeof(math::vec3), &data->tangents[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    mesh_handle mesh_cache::create_mesh(const std::string& name, const fs::view& file, mesh_import_settings settings)
    {
        id_type id = nameHash(name);
        if (m_meshes.contains(id))
            return { id };

        if (!file.is_valid() || !file.file_info().is_file)
            return { 0 };

        auto result = fs::AssetImporter::tryLoad<mesh_data>(file, settings);

        if (result != common::valid)
            return { 0 };

        mesh_data* data;
        {
            async::readwrite_guard guard(m_dataLock);
            data = m_meshdata.emplace<mesh_data>(id, result).first->get();
        }

        mesh mesh;
        //mesh.indexCount = submesh.indices.size();

        buffer(data, mesh);
        data->fileName = name;
        m_meshes.insert(id, mesh);
        return { id };
    }

    inline mesh_handle mesh_cache::get_handle(const std::string& name)
    {
        id_type id = nameHash(name);
        if (m_meshes.contains(id))
            return { id };
        return { 0 };
    }

    inline mesh_handle mesh_cache::get_handle(id_type id)
    {
        if (m_meshes.contains(id))
            return { id };
        return { 0 };
    }
}
