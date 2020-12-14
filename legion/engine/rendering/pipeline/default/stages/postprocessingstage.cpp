#include <rendering/pipeline/default/stages/postprocessingstage.hpp>

namespace legion::rendering
{

    std::multimap<priority_type, shader_handle> PostProcessingStage::m_shaders;

    void PostProcessingStage::addShader(shader_handle shaderToAdd, priority_type priority)
    {
        m_shaders.emplace(priority, shaderToAdd);
    }

    void PostProcessingStage::removeShader(shader_handle shaderToRemove)
    {
        for (auto iter = m_shaders.begin(); iter != m_shaders.end();)
        {
            const auto eraseIter = iter++;
            if (eraseIter->second == shaderToRemove)
            {
                m_shaders.erase(eraseIter);
            }
        }
    }

    void PostProcessingStage::setup(app::window& context)
    {

        float quadVertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
          // positions   // texCoords
          -1.0f,  1.0f,  0.0f, 1.0f,
          -1.0f, -1.0f,  0.0f, 0.0f,
           1.0f, -1.0f,  1.0f, 0.0f,

          -1.0f,  1.0f,  0.0f, 1.0f,
           1.0f, -1.0f,  1.0f, 0.0f,
           1.0f,  1.0f,  1.0f, 1.0f
        };
        app::context_guard guard(context);

        m_quadVAO = vertexarray::generate();
        m_quadVBO = buffer(GL_ARRAY_BUFFER,sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        m_quadVAO.setAttribPointer(m_quadVBO, 0, 2, GL_FLOAT, false, 4*sizeof(float), 0);
        m_quadVAO.setAttribPointer(m_quadVBO, 1, 2, GL_FLOAT, false, 4 * sizeof(float), 2 * sizeof(float));
    }

    void PostProcessingStage::render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
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


        auto a = fbo->getAttachment(GL_COLOR_ATTACHMENT0);
        if (!a.has_value())
        {
            log::error("Color attachment was not found.");
            return;
        }
        if (a.type() != typeid(texture_handle))
        {
            log::error("Color attachment needs to be a texture to be able to use it for post processing.");
            return;
        }

        texture_handle* texture = std::any_cast<texture_handle>(&a);
        fbo->bind();
        m_quadVAO.bind();

        for (auto [_,shader] : m_shaders)
        {
            shader.bind();
            shader.get_uniform<texture_handle>("screenTexture").set_value(*texture);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindTexture(GL_TEXTURE_2D, 0);
            shader.release();
        }
        m_quadVAO.release();
        fbo->release();
    }

    priority_type PostProcessingStage::priority()
    {
        return post_fx_priority;
    }

}
