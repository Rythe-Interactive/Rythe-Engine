#include <rendering/data/model.hpp>

#include <map>
#include <string>
#include <fstream>

namespace args::rendering
{
    sparse_map<id_type, model> model_cache::m_models;
    async::readonly_rw_spinlock model_cache::m_modelLock;

    sparse_map<id_type, std::unique_ptr<mesh>> model_cache::m_meshdata;
    async::readonly_rw_spinlock model_cache::m_dataLock;

    bool model_handle::is_buffered()
    {
        return model_cache::get_model(id).buffered;
    }

    void model_handle::buffer_data(app::gl_id matrixBuffer)
    {
        model_cache::buffer(id, matrixBuffer);
    }

    inline const mesh& model_handle::get_data()
    {
        return model_cache::get_data(id);
    }

    inline const model& model_handle::get_model()
    {
        return model_cache::get_model(id);
    }

    inline const model& model_cache::get_model(id_type id)
    {
        async::readonly_guard guard(m_modelLock);
        return m_models[id];
    }

    inline const mesh& model_cache::get_data(id_type id)
    {
        async::readonly_guard guard(m_dataLock);
        return *m_meshdata[id];
    }

    void model_cache::buffer(id_type id, app::gl_id matrixBuffer)
    {
        mesh* data;

        {
            async::readonly_guard guard(m_dataLock);
            data = m_meshdata[id].get();
        }

        {
            async::readonly_guard guard(m_modelLock);
            model& mesh = m_models[id];

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

    model_handle model_cache::create_model(const std::string& name, const fs::view& file, mesh_import_settings settings)
    {
        id_type id = nameHash(name);

        {
            async::readonly_guard guard(m_modelLock);
            if (m_models.contains(id))
                return { id };
        }

        if (!file.is_valid() || !file.file_info().is_file)
            return invalid_model_handle;

        auto result = fs::AssetImporter::tryLoad<mesh>(file, settings);

        if (result != common::valid)
            return invalid_model_handle;

        mesh* data;
        {
            async::readwrite_guard guard(m_dataLock);
            data = m_meshdata.emplace(id, new mesh(result)).first->get();
        }

        data->fileName = file.get_filename();

        model mesh{};

        for (auto& submeshData : data->submeshes)
            mesh.submeshes.push_back(submeshData);

        mesh.buffered = false;

        {
            async::readwrite_guard guard(m_modelLock);
            m_models.insert(id, mesh);
        }

        return { id };
    }

    inline model_handle model_cache::get_handle(const std::string& name)
    {
        id_type id = nameHash(name);
        async::readonly_guard guard(m_modelLock);
        if (m_models.contains(id))
            return { id };
        return invalid_model_handle;
    }

    inline model_handle model_cache::get_handle(id_type id)
    {
        async::readonly_guard guard(m_modelLock);
        if (m_models.contains(id))
            return { id };
        return invalid_model_handle;
    }
}
