#include <rendering/pipeline/default/defaultpipeline.hpp>
#include <rendering/pipeline/default/stages/clearstage.hpp>
#include <rendering/pipeline/default/stages/framebufferresizestage.hpp>
#include <rendering/pipeline/default/stages/lightbufferstage.hpp>
#include <rendering/pipeline/default/stages/meshbatchingstage.hpp>
#include <rendering/pipeline/default/stages/meshrenderstage.hpp>
#include <rendering/pipeline/default/stages/debugrenderstage.hpp>
#include <rendering/pipeline/default/stages/postprocessingstage.hpp>
#include <rendering/pipeline/default/stages/submitstage.hpp>
#include <rendering/pipeline/default/postfx/tonemapping.hpp>
#include <rendering/pipeline/default/postfx/fxaa.hpp>
#include <rendering/pipeline/default/postfx/bloom.hpp>
#include <rendering/pipeline/default/postfx/depthoffield.hpp>
#include <rendering/data/buffer.hpp>


namespace legion::rendering
{
    void DefaultPipeline::setup(app::window& context)
    {
        OPTICK_EVENT();
        attachStage<ClearStage>();
        attachStage<FramebufferResizeStage>();
        attachStage<LightBufferStage>();
        attachStage<MeshBatchingStage>();
        attachStage<MeshRenderStage>();
        attachStage<DebugRenderStage>();
        attachStage<PostProcessingStage>();
        attachStage<SubmitStage>();

        PostProcessingStage::addEffect<Tonemapping>();
        PostProcessingStage::addEffect<Bloom>(-64);
        PostProcessingStage::addEffect<FXAA>(-100);
        PostProcessingStage::addEffect<DepthOfField>(-101);

        buffer modelMatrixBuffer;

        {
            app::context_guard guard(context);
            addFramebuffer("main");
            modelMatrixBuffer = buffer(GL_ARRAY_BUFFER, sizeof(math::mat4) * 1024, nullptr, GL_DYNAMIC_DRAW);
        }

        create_meta<buffer>("model matrix buffer", modelMatrixBuffer);
    }

}
