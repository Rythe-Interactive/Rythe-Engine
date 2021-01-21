#pragma once
#include <rendering/data/postprocessingeffect.hpp>

namespace legion::rendering
{
    class DepthOfField : public PostProcessingEffect<DepthOfField>
    {
    private:
        //Shaders needed.
        shader_handle m_depthThresholdShader;
        shader_handle m_bokehShader;
        shader_handle m_screenShader;
        shader_handle m_combineShader;
        shader_handle m_postFilterShader;
        shader_handle m_preFilterShader;
        //Area of Focus pass Fbo and texture.
        framebuffer m_thresholdFbo;
        texture_handle m_thresholdTexture;
        //Bokeh pass textures.
        texture_handle m_destinationTexture;
        texture_handle m_halfres1;
        texture_handle m_halfres2;
        math::ivec2 m_halfres;

        float m_bokehSize;

        texture_import_settings settings{
              texture_type::two_dimensional,
              channel_format::float_hdr,
              texture_format::rgba_hdr,
              texture_components::rgb,
              true,
              true,
              texture_mipmap::linear,
              texture_mipmap::linear,
              texture_wrap::mirror,
              texture_wrap::mirror,
              texture_wrap::mirror
        };
    public:
        /**
         * @brief setup The setup function of the post processing effect.
         * @param context The current context that is being used inside of the effect.
         */
        void setup(app::window& context) override;
        /**
         * @brief renderPass The function that is called every frame.
         * @param fbo The framebuffer used for this particular effect.
         * @param colortexture The lighting data gathered from previous renderpasses.
         * @param depthtexture The depth data gathered from previous renderpasses.
         * @param deltaTime Current delta time.
         */
        void renderPass(framebuffer& fbo, RenderPipelineBase* pipeline, camera& cam, const camera::camera_input& camInput, time::span deltaTime);

        void areaOfFocus(texture_handle& color_texture, const camera::camera_input& camInput, texture_handle& position_texture);

        void preFilter(framebuffer& fbo, texture_handle& color_texture);

        void bokeh(framebuffer& fbo, texture_handle& color_texture);

        void postFilter(framebuffer& fbo, texture_handle& color_texture);

        void combine(framebuffer& fbo, texture_handle& color_texture);

        void renderResults(framebuffer& fbo, texture_handle& texture, texture_handle& color_texture);

        std::tuple<bool, texture_handle, texture_handle> getTextures(framebuffer& fbo);
    };
}

