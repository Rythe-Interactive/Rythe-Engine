#include <rendering/pipeline/default/stages/meshrenderstage.hpp>
#include <rendering/components/light.hpp>
#include <rendering/data/buffer.hpp>

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

        auto* batches = get_meta<sparse_map<material_handle, sparse_map<model_handle, std::unordered_set<ecs::entity_handle>>>>(batchesId);
        if (!batches)
            return;

        buffer* lightsBuffer = get_meta<buffer>(lightsId);
        if (!lightsBuffer)
            return;

        size_type* lightCount = get_meta<size_type>(lightCountId);
        if (!lightCount)
            return;

        buffer* modelMatrixBuffer = get_meta<buffer>(matricesId);
        if (!modelMatrixBuffer)
            return;

        auto fbo = getFramebuffer(mainId);

        app::context_guard guard(context);

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

                m_matrices.resize(instances.size());
                int i = 0;
                for (auto& ent : instances)
                {
                    m_matrices[i] = transform(ent.get_component_handles<transform>()).get_local_to_world_matrix();
                    i++;
                }

                modelMatrixBuffer->bufferData(m_matrices);

                mesh.vertexArray.bind();
                mesh.indexBuffer.bind();
                lightsBuffer->bind();
                for (auto submesh : mesh.submeshes)
                    glDrawElementsInstanced(GL_TRIANGLES, (GLuint)submesh.indexCount, GL_UNSIGNED_INT, (GLvoid*)(submesh.indexOffset * sizeof(uint)), (GLsizei)instances.size());

                lightsBuffer->release();
                mesh.indexBuffer.release();
                mesh.vertexArray.release();
            }

            material.release();
        }

        fbo.release();
    }
}
