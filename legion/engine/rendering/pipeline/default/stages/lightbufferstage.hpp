#pragma once
#include <rendering/pipeline/base/renderstage.hpp>
#include <rendering/pipeline/base/pipeline.hpp>
#include <rendering/components/light.hpp>

namespace legion::rendering
{
    class LightBufferStage : public RenderStage<LightBufferStage>
    {
        sparse_map<ecs::entity_handle, detail::light_data> lights;

        void onLightCreate(events::component_creation<light>* event);
        void onLightDestroy(events::component_destruction<light>* event);

        virtual void setup(app::window& context) override;
        virtual void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) override;
        virtual priority_type priority() override;
    };
}
