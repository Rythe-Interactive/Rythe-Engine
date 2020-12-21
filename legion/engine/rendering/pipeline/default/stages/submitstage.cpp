#include <rendering/pipeline/default/stages/submitstage.hpp>

using namespace legion::core::filesystem::literals;

namespace legion::rendering
{

    void SubmitStage::setup(app::window& context)
    {
        OPTICK_EVENT();
        app::context_guard guard(context);
        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
          // positions         // texCoords
          -1.0f,  1.0f,  1.0,  0.0f, 1.0f,
          -1.0f, -1.0f,  1.0,  0.0f, 0.0f,
           1.0f, -1.0f,  1.0,  1.0f, 0.0f,

          -1.0f,  1.0f,  1.0,  0.0f, 1.0f,
           1.0f, -1.0f,  1.0,  1.0f, 0.0f,
           1.0f,  1.0f,  1.0,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &m_quadVAO);
        glGenBuffers(1, &m_quadVBO);
        glBindVertexArray(m_quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(SV_POSITION);
        glVertexAttribPointer(SV_POSITION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(SV_TEXCOORD0);
        glVertexAttribPointer(SV_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        m_screenShader = ShaderCache::create_shader("screen shader", "engine://shaders/screenshader.shs"_view);
    }

    void SubmitStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
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
        if (!guard.contextIsValid())
        {
            abort();
            return;
        }

        auto viewportSize = context.framebufferSize();

        auto colorAttachment = fbo->getAttachment(GL_COLOR_ATTACHMENT0);
        if (std::holds_alternative<std::monostate>(colorAttachment) || !std::holds_alternative<texture_handle>(colorAttachment))
        {
            log::error("Color attachment of main FBO was invalid.");
            return;
        }

        auto screenTexture = std::get<texture_handle>(colorAttachment);

        if (cam.renderTarget.id() != 0)
        {
            auto [success, message] = cam.renderTarget.verify();
            if (!success)
            {
                log::error(message);
                return;
            }
        }

        cam.renderTarget.bind();

        glViewport(0, 0, viewportSize.x, viewportSize.y);
        glDisable(GL_DEPTH_TEST);

        m_screenShader.bind();
        m_screenShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(screenTexture);
        glBindVertexArray(m_quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);

        cam.renderTarget.release();
    }

    priority_type SubmitStage::priority()
    {
        return submit_priority;
    }
}
