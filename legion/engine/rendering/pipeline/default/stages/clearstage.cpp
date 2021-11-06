#include <rendering/pipeline/default/stages/clearstage.hpp>

namespace legion::rendering
{
    void ClearStage::setup(app::window& context)
    {
        OPTICK_EVENT();
        app::context_guard guard(context);
        glClearDepth(1.0f);
    }

    void ClearStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        OPTICK_EVENT();
        (void)deltaTime;
        (void)camInput;

        static id_type mainId = nameHash("main");

        auto fbo = getFramebuffer(mainId);
        if (!fbo)
        {
            log::error("Main frame buffer is missing.");
            abort();
            return;
        }

        app::context_guard guard(context);

        auto [valid, message] = fbo->verify();
        if (!valid)
        {
            log::error("Main frame buffer isn't complete: {}", message);
            abort();
            return;
        }

        fbo->bind();

        uint attachment = FRAGMENT_ATTACHMENT;
        glDrawBuffers(1, &attachment);

        glClearColor(cam.clearColor.r, cam.clearColor.g, cam.clearColor.b, cam.clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        uint metaAttachments[3] = { NORMAL_ATTACHMENT, POSITION_ATTACHMENT, OVERDRAW_ATTACHMENT };
        glDrawBuffers(3, metaAttachments);

        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        uint attachments[4] = { FRAGMENT_ATTACHMENT, NORMAL_ATTACHMENT, POSITION_ATTACHMENT, OVERDRAW_ATTACHMENT };
        glDrawBuffers(4, attachments);

        fbo->release();
    }

    priority_type ClearStage::priority()
    {
        return setup_priority;
    }
}
