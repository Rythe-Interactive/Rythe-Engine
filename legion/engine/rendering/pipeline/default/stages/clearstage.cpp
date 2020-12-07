#include <rendering/pipeline/default/stages/clearstage.hpp>

namespace legion::rendering
{
    void ClearStage::setup(app::window& context)
    {
        app::contextguard guard(context);
        glClearDepth(0.0f);
    }

    void ClearStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        (void)deltaTime;
        static id_type mainId = nameHash("main");

        auto fbo = m_pipeline->getFramebuffer(mainId);

        app::contextguard guard(context);

        fbo.bind();

        glClearColor(cam.clearColor.r, cam.clearColor.g, cam.clearColor.b, cam.clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        fbo.release();
    }

    priority_type ClearStage::priority()
    {
        return PRIORITY_MAX;
    }
}
