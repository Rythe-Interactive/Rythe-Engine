#include <rendering/pipeline/default/defaultpipeline.hpp>
#include <rendering/pipeline/default/stages/meshrenderstage.hpp>

namespace legion::rendering
{
    void DefaultPipeline::setup()
    {
        addFramebuffer("main");
        attachStage<MeshRenderStage>();
    }

}
