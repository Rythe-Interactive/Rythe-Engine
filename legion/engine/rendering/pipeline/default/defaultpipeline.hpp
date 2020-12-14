#pragma once
#include <rendering/pipeline/base/pipeline.hpp>

namespace legion::rendering
{
    class DefaultPipeline : public RenderPipeline<DefaultPipeline>
    {
        virtual void setup(app::window& context) override;
    };
}
