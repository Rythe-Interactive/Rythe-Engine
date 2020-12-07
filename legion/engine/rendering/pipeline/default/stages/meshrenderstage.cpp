#include <rendering/pipeline/default/stages/meshrenderstage.hpp>
#include <rendering/components/light.hpp>

namespace legion::rendering
{
    void MeshRenderStage::setup(app::window& context)
    {
    }

    void MeshRenderStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        (void)deltaTime;
        (void)cam;
        static id_type mainId = nameHash("main");
        static id_type batchesId = nameHash("mesh batches");
        static id_type lightsId = nameHash("light buffer");
        static id_type lightCountId = nameHash("light count");
        static id_type matricesId = nameHash("model matrix buffer");

        auto* batches = m_pipeline->get_meta<sparse_map<material_handle, sparse_map<model_handle, std::vector<math::mat4>>>>(batchesId);
        if (!batches)
            return;

        app::gl_id* lightsBuffer = m_pipeline->get_meta<app::gl_id>(lightsId);
        if (!lightsBuffer)
            return;

        size_type* lightCount = m_pipeline->get_meta<size_type>(lightCountId);
        if (!lightCount)
            return;

        app::gl_id* modelMatrixBuffer = m_pipeline->get_meta<app::gl_id>(matricesId);
        if (!modelMatrixBuffer)
            return;

        auto fbo = m_pipeline->getFramebuffer(mainId);

        app::contextguard guard(context);

        fbo.bind();

        for (auto [material, instancesPerMaterial] : *batches)
        {
            camInput.bind(material);
            if (material.has_param<uint>(SV_LIGHT_COUNT))
                material.set_param<uint>(SV_LIGHT_COUNT, *lightCount);
            material.bind();

            for (auto [modelHandle, instances] : instancesPerMaterial)
            {
                if (!modelHandle.is_buffered())
                    modelHandle.buffer_data(*modelMatrixBuffer);

                model mesh = modelHandle.get_model();
                if (mesh.submeshes.empty())
                {
                    log::warn("Empty mesh found.");
                    continue;
                }

                glBindBuffer(GL_ARRAY_BUFFER, *modelMatrixBuffer);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(math::mat4) * instances.size(), instances.data());
                glBindBuffer(GL_ARRAY_BUFFER, 0);

                glBindVertexArray(mesh.vertexArrayId);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexBufferId);
                glBindBuffer(GL_SHADER_STORAGE_BUFFER, *lightsBuffer);
                glBindTexture(GL_TEXTURE_2D, 0);
                for (auto submesh : mesh.submeshes)
                    glDrawElementsInstanced(GL_TRIANGLES, (GLuint)submesh.indexCount, GL_UNSIGNED_INT, (GLvoid*)(submesh.indexOffset * sizeof(uint)), (GLsizei)instances.size());

                glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }

            material.release();
        }

        fbo.release();
    }
}
