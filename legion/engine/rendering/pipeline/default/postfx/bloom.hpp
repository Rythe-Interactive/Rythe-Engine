#pragma once
#include <rendering/data/postprocessingeffect.hpp>


namespace legion::rendering
{
    /**
     * @class Bloom
     * @brief A bloom post processing effect.
     */
    class Bloom : public PostProcessingEffect<Bloom>
    {
    private:
        shader_handle m_brightnessThresholdShader;
        shader_handle m_gaussianBlurShader;
        shader_handle m_combineShader;
        shader_handle m_historyMixShader;

        framebuffer m_pingpongFrameBuffers[2];
        texture_handle m_pingpongTextureBuffers[2];

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

        void seperateOverdraw(framebuffer& fbo, texture_handle colortexture, texture_handle overdrawtexture);

        texture_handle blurOverdraw(const math::ivec2& framebufferSize, texture_handle overdrawtexture);

        void historyMixOverdraw(framebuffer& fbo, texture_handle overdrawtexture);

        void combineImages(framebuffer& fbo, texture_handle colortexture, texture_handle overdrawtexture);

        /**
         * @brief renderPass The function that is called every frame.
         * @param fbo The framebuffer used for this particular effect.
         * @param colortexture The lighting data gathered from previous renderpasses.
         * @param depthtexture The depth data gathered from previous renderpasses.
         * @param deltaTime Current delta time.
         */
        void renderPass(framebuffer& fbo, RenderPipelineBase* pipeline, camera& cam, const camera::camera_input& camInput, time::span deltaTime);
    };
}
