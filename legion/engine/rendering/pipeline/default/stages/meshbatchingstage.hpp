#pragma once
#include <rendering/pipeline/base/renderstage.hpp>
#include <rendering/pipeline/base/pipeline.hpp>
#include <rendering/components/renderable.hpp>

namespace legion::rendering
{
    class MeshBatchingStage : public RenderStage<MeshBatchingStage>
    {
        sparse_map<ecs::entity_handle, std::pair<material_handle, model_handle>> m_instances;

        async::spinlock m_insertionLock;
        ecs::entity_set m_toInsert;
        async::spinlock m_erasureLock;
        ecs::entity_set m_toErase;
        async::spinlock m_reinsertionLock;
        ecs::entity_set m_toReinsert;

        void eraseInstance(ecs::entity_handle entity);
        void insertInstance(ecs::entity_handle entity);
        void reinsertInstance(ecs::entity_handle entity);

        void onRendererCreate(events::component_creation<mesh_renderer>* event);
        void onRendererDestroy(events::component_destruction<mesh_renderer>* event);
        void onRendererModified(events::component_modification<mesh_renderer>* event);
        void onFilterCreate(events::component_creation<mesh_filter>* event);
        void onFilterDestroy(events::component_destruction<mesh_filter>* event);
        void onFilterModified(events::component_modification<mesh_filter>* event);

    public:
        virtual void setup(app::window& context) override;
        virtual void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) override;
        virtual priority_type priority() override;
    };
}
