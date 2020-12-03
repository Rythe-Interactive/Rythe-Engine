#include <rendering/pipeline/default/defaultpipeline.hpp>
#include <rendering/pipeline/default/mainrenderstage.hpp>

namespace legion::rendering
{
    void DefaultPipeline::setup()
    {
        addFramebuffer("main");
        attachStage<MainRenderState>();
    }

}
