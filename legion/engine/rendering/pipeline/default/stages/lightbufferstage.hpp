#pragma once
#include <rendering/pipeline/base/renderstage.hpp>
#include <rendering/pipeline/base/pipeline.hpp>
#include <rendering/components/light.hpp>

namespace legion::rendering
{
    class LightBufferStage : public RenderStage<LightBufferStage>
    {
        static async::spinlock m_lightEntitiesLock;
        static std::unordered_set<ecs::entity> m_lightEntities;
        static std::vector<detail::light_data> m_lights;

        void onLightCreate(events::component_creation<light>& event);
        void onLightDestroy(events::component_destruction<light>& event);

    public:
        void shutdown()
        {
            m_lightEntities.clear();
            m_lights.clear();
        }

        virtual void setup(app::window& context) override;
        virtual void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) override;
        virtual priority_type priority() override;
    };
}
