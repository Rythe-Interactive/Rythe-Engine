#include <rendering/pipeline/default/defaultpipeline.hpp>
#include <rendering/pipeline/default/stages/meshrenderstage.hpp>
#include <rendering/data/buffer.hpp>

namespace legion::rendering
{
    void DefaultPipeline::setup(app::window& context)
    {
        addFramebuffer("main");
        attachStage<MeshRenderStage>();

        buffer modelMatrixBuffer;

        {
            app::context_guard guard(context);
            modelMatrixBuffer = buffer(GL_ARRAY_BUFFER, sizeof(math::mat4) * 1024, nullptr, GL_DYNAMIC_DRAW);
        }

        create_meta<buffer>("model matrix buffer", modelMatrixBuffer);
    }

}
