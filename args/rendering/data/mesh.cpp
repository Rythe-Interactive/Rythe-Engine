#include <rendering/data/mesh.hpp>

#include <map>
#include <string>
#include <fstream>

namespace args::rendering
{
    atomic_sparse_map<id_type, mesh> mesh_cache::m_meshes;
    sparse_map<id_type, std::unique_ptr<mesh_data>> mesh_cache::m_meshdata;
    async::readonly_rw_spinlock mesh_cache::m_dataLock;

    void mesh_handle::bind(attribute* verticesAttrib, attribute* normalsAttrib, attribute* uvAttrib, attribute* tangentsAttrib) const
    {
        mesh mesh = mesh_cache::get_mesh(id);

        if (verticesAttrib && verticesAttrib->IsValid()) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferId);
            glEnableVertexAttribArray(verticesAttrib->GetLocation());
            glVertexAttribPointer(verticesAttrib->GetLocation(), 3, GL_FLOAT, GL_FALSE, 0, 0);
        }

        if (normalsAttrib && normalsAttrib->IsValid()) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.normalBufferId);
            glEnableVertexAttribArray(normalsAttrib->GetLocation());
            glVertexAttribPointer(normalsAttrib->GetLocation(), 3, GL_FLOAT, GL_TRUE, 0, 0);
        }

        if (uvAttrib && uvAttrib->IsValid()) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.uvBufferId);
            glEnableVertexAttribArray(uvAttrib->GetLocation());
            glVertexAttribPointer(uvAttrib->GetLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
        }

        if (tangentsAttrib && tangentsAttrib->IsValid()) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh.tangentBufferId);
            glEnableVertexAttribArray(tangentsAttrib->GetLocation());
            glVertexAttribPointer(tangentsAttrib->GetLocation(), 3, GL_FLOAT, GL_TRUE, 0, 0);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    }

    void mesh_handle::draw(unsigned count) const
    {
        mesh mesh = mesh_cache::get_mesh(id);

        glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)mesh.indexCount, GL_UNSIGNED_INT, (GLvoid*)0, (GLsizei)count);
    }

    void mesh_handle::unbind(attribute* verticesAttrib, attribute* normalsAttrib, attribute* uvAttrib, attribute* tangentsAttrib)
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        //fix for serious performance issue
        if (tangentsAttrib && tangentsAttrib->IsValid()) glDisableVertexAttribArray(tangentsAttrib->GetLocation());
        if (uvAttrib && uvAttrib->IsValid()) glDisableVertexAttribArray(uvAttrib->GetLocation());
        if (normalsAttrib && normalsAttrib->IsValid()) glDisableVertexAttribArray(normalsAttrib->GetLocation());
        if (verticesAttrib && verticesAttrib->IsValid()) glDisableVertexAttribArray(verticesAttrib->GetLocation());
    }

    mesh_data* mesh_handle::get_data()
    {
        return nullptr;
    }

    inline mesh mesh_cache::get_mesh(id_type id)
    {
        return m_meshes[id]->load(std::memory_order_acquire);
    }

    void mesh_cache::buffer(mesh_data* data, mesh& mesh)
    {
        glGenBuffers(1, &mesh.indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indices.size() * sizeof(unsigned int), &data->indices[0], GL_STATIC_DRAW);

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

    void mesh_cache::calculate_tangents(mesh_data* data)
    {
        for (unsigned i = 0; i < data->normals.size(); i++)
            data->tangents.push_back(math::vec3(0));

        for (unsigned i = 0; i < data->indices.size(); i += 3)
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
            tangent.x = uvDetFrac * (deltaUV1.y * edge0.x - deltaUV0.y * edge1.x);
            tangent.y = uvDetFrac * (deltaUV1.y * edge0.y - deltaUV0.y * edge1.y);
            tangent.z = uvDetFrac * (deltaUV1.y * edge0.z - deltaUV0.y * edge1.z);
            tangent = math::normalize(tangent);

            data->tangents[data->indices[i]] += tangent;
            data->tangents[data->indices[i + 1]] += tangent;
            data->tangents[data->indices[i + 2]] += tangent;
        }

        for (unsigned i = 0; i < data->tangents.size(); i++)
            data->tangents[i] = math::normalize(data->tangents[i]);
    }

    mesh_handle mesh_cache::create_mesh(const std::string& name, const fs::view& file, mesh_import_settings settings)
    {
        id_type id = nameHash(name);
        if (m_meshes.contains(id))
            return { id };

        if (!file.is_valid() || !file.file_info().is_file)
            return { 0 };

        auto result = filesystem::AssetImporter::tryLoad<mesh_data>(file, settings);

        if (result != common::valid)
            return { 0 };

        mesh_data* data;
        {
            async::readwrite_guard guard(m_dataLock);
            data = m_meshdata.emplace<mesh_data>(id, result).first->get();
        }

        if (!load_mesh_data(data, file, settings))
            return { 0 };

        mesh mesh;
        mesh.indexCount = data->indices.size();

        calculate_tangents(data);
        buffer(data, mesh);
        data->name = name;
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
