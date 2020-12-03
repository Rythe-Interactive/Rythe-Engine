#pragma once
#include <rendering/pipeline/base/renderstage.hpp>
#include <rendering/pipeline/base/pipeline.hpp>

namespace legion::rendering
{
    class MainRenderState : public RenderStage
    {
        virtual void setup();
        virtual void render(app::window& context, camera& cam, time::span deltaTime);
    };
}
