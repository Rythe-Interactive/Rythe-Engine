#pragma once
#include <rendering/pipeline/base/renderstage.hpp>

namespace legion::rendering
{
    class ImGuiStage : public RenderStage<ImGuiStage>
    {
    public:
        void setup(app::window& context) override;
        void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) override;
        priority_type priority() override;

        static multicast_delegate<void()> OnGuiRender;
    };
}
