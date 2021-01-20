#include <rendering/pipeline/default/postfx/depthoffield.hpp>
#include <rendering/systems/renderer.hpp>
#include <core/filesystem/filesystem.hpp>


namespace legion::rendering
{
    void DepthOfField::setup(app::window& context)
    {
        using namespace fs::literals;
        // Create all the shaders needed.
        m_depthThresholdShader = rendering::ShaderCache::create_shader("depth threshold", "engine://shaders/depththreshold.shs"_view);
        m_bokehShader = rendering::ShaderCache::create_shader("dof bokeh", "engine://shaders/dofbokeh.shs"_view);
        m_screenShader = ShaderCache::create_shader("screenshader", "engine://shaders/screenshader.shs"_view);
        m_downscaleShader = ShaderCache::create_shader("downscaleshader", "engine://shaders/downscaleshader.shs"_view);
        m_postFilterShader = ShaderCache::create_shader("postfiltershader", "engine://shaders/postfilter.shs"_view);
        m_preFilterShader = ShaderCache::create_shader("prefiltershader", "engine://shaders/prefilter.shs"_view);

        // Create threshold fbo and texture.
        m_thresholdFbo = framebuffer(GL_FRAMEBUFFER);
        m_thresholdTexture = rendering::TextureCache::create_texture("dofthreshold", context.size(), settings);
        m_thresholdFbo.attach(m_thresholdTexture, FRAGMENT_ATTACHMENT);

        // Create bokeh textures and resolution.
        m_destinationTexture = rendering::TextureCache::create_texture("dofdestination", context.size(), settings);
        m_halfres = context.size()/2;
        m_halfres1 = rendering::TextureCache::create_texture("bokehhalfres1", m_halfres, settings);
        m_halfres2 = rendering::TextureCache::create_texture("bokehhalfres2", m_halfres, settings);

        // Adding itself to the post processing renderpass.
        addRenderPass<&DepthOfField::renderPass>();
    }
    void DepthOfField::renderPass(framebuffer& fbo, RenderPipelineBase* pipeline, camera& cam, const camera::camera_input& camInput, time::span deltaTime)
    {
        //Gets textures from framebuffer.
        auto [valid_textures, position_texture, color_texture] = getTextures(fbo);
        if (!valid_textures) return;

        //Change sizes
        math::ivec2 textureSize = color_texture.get_texture().size();
        if (textureSize != m_destinationTexture.get_texture().size())
        {
            m_destinationTexture.get_texture().resize(textureSize);
            m_thresholdTexture.get_texture().resize(textureSize);
            m_halfres = textureSize / 2;
            m_halfres1.get_texture().resize(m_halfres);
            m_halfres2.get_texture().resize(m_halfres);
        }

        //Calculates the area of focus and outputs it onto a texture.
        areaOfFocus(color_texture, camInput, position_texture);
        //Downsizing areaOfFocus.
        preFilter(fbo, color_texture);
        //Uses bokeh blurring to blur low res color_texture.
        bokeh(fbo, color_texture);
        //Post blur filtering.
        postFilter(fbo, color_texture);


        //Renders the resulting texture onto the screen.
        renderResults(fbo, m_destinationTexture);
    }

    std::tuple<bool, texture_handle, texture_handle> DepthOfField::getTextures(framebuffer& fbo)
    {
        //Try to get position attachment.
        auto position_attachment = fbo.getAttachment(POSITION_ATTACHMENT);
        if (!std::holds_alternative<texture_handle>(position_attachment)) return std::make_tuple(false, invalid_texture_handle, invalid_texture_handle);

        //Try to get color attachment.
        auto color_attachment = fbo.getAttachment(FRAGMENT_ATTACHMENT);
        if (!std::holds_alternative<texture_handle>(color_attachment)) return std::make_tuple(false, invalid_texture_handle, invalid_texture_handle);

        //Get position texture.
        auto position_texture = std::get<texture_handle>(position_attachment);

        //Get color texture.
        auto color_texture = std::get<texture_handle>(color_attachment);

        return std::make_tuple(true, position_texture, color_texture);
    }

    void DepthOfField::renderResults(framebuffer& fbo,texture_handle& texture)
    {
        //Binds and assigns.
        fbo.bind();
        m_screenShader.bind();
        m_screenShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(texture);
        //Render it onto a quad.
        renderQuad();
        //RELEASE THE KRAKEN.
        m_screenShader.release();
        fbo.release();
    }

    void DepthOfField::areaOfFocus(texture_handle& color_texture, const camera::camera_input& camInput, texture_handle& position_texture)
    {
        // Brightness threshold stage.
        m_thresholdFbo.bind();
        // Add color attachment to fbo.
        uint attachment = FRAGMENT_ATTACHMENT;
        glDrawBuffers(1, &attachment);

        // Bind and assign the depth threshold shader.
        m_depthThresholdShader.bind();
        m_depthThresholdShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(color_texture);
        m_depthThresholdShader.get_uniform_with_location<math::vec4>(SV_VIEWDIR).set_value(camInput.vdirfarz);
        m_depthThresholdShader.get_uniform_with_location<math::vec4>(SV_CAMPOS).set_value(camInput.posnearz);
        m_depthThresholdShader.get_uniform_with_location<math::mat4>(SV_VIEW).set_value(camInput.view);
        m_depthThresholdShader.get_uniform_with_location<texture_handle>(SV_SCENEPOSITION).set_value(position_texture);
        m_depthThresholdShader.get_uniform<float>("sampleOffset").set_value(0.5f);
        m_depthThresholdShader.get_uniform<float>("focalOffset").set_value(5.f);
        m_depthThresholdShader.get_uniform<float>("bokehRadius").set_value(4.f);

        // Render onto the quad.
        renderQuad();
        // Release the shader and threshold fbo.
        m_depthThresholdShader.release();
        m_thresholdFbo.release();
    }

    void DepthOfField::bokeh(framebuffer& fbo, texture_handle& color_texture)
    {
        //Pre-Bokeh
        fbo.attach(m_halfres1, FRAGMENT_ATTACHMENT);
        fbo.bind();
        m_downscaleShader.bind();
        m_downscaleShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(color_texture);
        m_downscaleShader.get_uniform<math::vec2>("scale").set_value(math::vec2(0.5f));
        renderQuad();
        m_downscaleShader.release();
        fbo.release();

        //Bokeh blur
        fbo.attach(m_halfres2, FRAGMENT_ATTACHMENT);
        fbo.bind();
        m_bokehShader.bind();
        m_bokehShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(m_halfres1);
        m_bokehShader.get_uniform<math::vec2>("scale").set_value(math::vec2(0.5f));
        m_bokehShader.get_uniform<float>("bokehRadius").set_value(4.0f);
        renderQuad();
        m_bokehShader.release();
        fbo.release();
        fbo.attach(color_texture, FRAGMENT_ATTACHMENT);
    }

    void DepthOfField::postFilter(framebuffer& fbo, texture_handle& color_texture)
    {
        fbo.attach(m_destinationTexture, FRAGMENT_ATTACHMENT);
        fbo.bind();
        m_postFilterShader.bind();
        m_postFilterShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(m_halfres2);
        //m_postFilterShader.get_uniform<math::vec2>("scale").set_value(math::vec2(2.f));
        renderQuad();
        m_postFilterShader.release();
        fbo.release();
        fbo.attach(color_texture, FRAGMENT_ATTACHMENT);
    }

    void DepthOfField::preFilter(framebuffer& fbo, texture_handle& color_texture)
    {
        fbo.attach(m_halfres1, FRAGMENT_ATTACHMENT);
        fbo.bind();
        m_preFilterShader.bind();
        m_preFilterShader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(color_texture);
        m_preFilterShader.get_uniform<math::vec2>("scale").set_value(math::vec2(0.5f));
        m_preFilterShader.get_uniform<texture_handle>("aofTexture").set_value(m_thresholdTexture);
        renderQuad();
        m_preFilterShader.release();
        fbo.release();
        fbo.attach(color_texture, FRAGMENT_ATTACHMENT);
    }
}
