#pragma once
#include <rendering/pipeline/base/renderstage.hpp>

namespace legion::rendering
{
    class MainRenderState : public RenderStage
    {
        virtual void setup();
        virtual void render(app::window context, camera cam);
    };
}
