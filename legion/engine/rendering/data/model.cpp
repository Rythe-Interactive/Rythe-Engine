#include <rendering/data/model.hpp>

#include <map>
#include <string>
#include <fstream>

namespace legion::rendering
{
    sparse_map<id_type, model> ModelCache::m_models;
    async::readonly_rw_spinlock ModelCache::m_modelLock;

    bool model_handle::is_buffered()
    {
        return ModelCache::get_model(id).buffered;
    }

    void model_handle::buffer_data(app::gl_id matrixBuffer)
    {
        ModelCache::buffer(id, matrixBuffer);
    }

    mesh_handle model_handle::get_mesh()
    {
        return ModelCache::get_mesh(id);
    }

    const model& model_handle::get_model()
    {
        return ModelCache::get_model(id);
    }

    const model& ModelCache::get_model(id_type id)
    {
        async::readonly_guard guard(m_modelLock);
        return m_models[id];
    }

    void ModelCache::buffer(id_type id, app::gl_id matrixBuffer)
    {
        if (id == invalid_id)
            return;

        auto mesh_handle = MeshCache::get_handle(id);
        if (!mesh_handle)
            return;
        auto [lock, mesh] = mesh_handle.get();

        async::readonly_multiguard guard(m_modelLock, lock);
        model& model = m_models[id];

        glGenVertexArrays(1, &model.vertexArrayId);
        glBindVertexArray(model.vertexArrayId);

        glGenBuffers(1, &model.indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint), mesh.indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glGenBuffers(1, &model.vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, model.vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(math::vec3), mesh.vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(SV_POSITION);
        glVertexAttribPointer(SV_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glGenBuffers(1, &model.colorBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, model.colorBufferId);
        glBufferData(GL_ARRAY_BUFFER, mesh.colors.size() * sizeof(math::color), mesh.colors.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(SV_COLOR);
        glVertexAttribPointer(SV_COLOR, 4, GL_FLOAT, GL_FALSE, 0, 0);

        glGenBuffers(1, &model.normalBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, model.normalBufferId);
        glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(math::vec3), mesh.normals.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(SV_NORMAL);
        glVertexAttribPointer(SV_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glGenBuffers(1, &model.tangentBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, model.tangentBufferId);
        glBufferData(GL_ARRAY_BUFFER, mesh.tangents.size() * sizeof(math::vec3), mesh.tangents.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(SV_TANGENT);
        glVertexAttribPointer(SV_TANGENT, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glGenBuffers(1, &model.uvBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, model.uvBufferId);
        glBufferData(GL_ARRAY_BUFFER, mesh.uvs.size() * sizeof(math::vec2), mesh.uvs.data(), GL_STATIC_DRAW);
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

        model.buffered = true;
    }

    model_handle ModelCache::create_model(const std::string& name, const fs::view& file, mesh_import_settings settings)
    {
        id_type id = nameHash(name);

        {// Check if the model already exists.
            async::readonly_guard guard(m_modelLock);
            if (m_models.contains(id))
                return { id };
        }

        // Check if the file is valid to load.
        if (!file.is_valid() || !file.file_info().is_file)
            return invalid_model_handle;

        model model{};
        std::string meshName;

        {// Load the mesh if it wasn't already. (It's called MeshCache for a reason.)
            auto handle = MeshCache::create_mesh(name, file, settings);
            if (handle == invalid_mesh_handle)
            {
                log::error("Failed to load model {}", name);
                return invalid_model_handle;
            }

            // Copy the sub-mesh data.
            auto [lock, data] = handle.get();
            async::readonly_guard guard(lock);
            meshName = data.fileName;

            for (auto& submeshData : data.submeshes)
                model.submeshes.push_back(submeshData);
        }

        // The model still needs to be buffered on the rendering thread.
        model.buffered = false;

        { // Insert the model into the model list.
            async::readwrite_guard guard(m_modelLock);
            m_models.insert(id, model);
        }

        log::debug("Created model {} with mesh: {}", name, meshName);

        return { id };
    }

    model_handle ModelCache::get_handle(const std::string& name)
    {
        id_type id = nameHash(name);
        async::readonly_guard guard(m_modelLock);
        if (m_models.contains(id))
            return { id };
        return invalid_model_handle;
    }

    model_handle ModelCache::get_handle(id_type id)
    {
        async::readonly_guard guard(m_modelLock);
        if (m_models.contains(id))
            return { id };
        return invalid_model_handle;
    }

    mesh_handle ModelCache::get_mesh(id_type id)
    {
        return MeshCache::get_handle(id);
    }

    mesh_handle ModelCache::get_mesh(const std::string& name)
    {
        id_type id = nameHash(name);
        return MeshCache::get_handle(id);
    }
}
