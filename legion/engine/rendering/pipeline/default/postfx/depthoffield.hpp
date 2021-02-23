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
        //Spread of the bokeh effect.
        float m_bokehSize = 4.f;
        //Standard settings for creating textures.
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
         * @param pipeline The pipeline used for this particular effect.
         * @param cam The camera object holding the camera data.
         * @param camInput The matrix data used to do everything 3D math-i.
         * @param deltaTime Current delta time.
         */
        void renderPass(framebuffer& fbo, RenderPipelineBase* pipeline, camera& cam, const camera::camera_input& camInput, time::span deltaTime);
        /**
         * @brief areaOfFocus Calculates what is in focus and what is not.
         * @param color_texture The scene color texture collected from the framebuffer.
         * @param camInput The matrix data used to do everything 3D math-i.
         * @param position_texture The scene position texture collected from the framebuffer.
         */
        void areaOfFocus(texture_handle& color_texture, const camera::camera_input& camInput, texture_handle& position_texture);
        /**
         * @brief preFilter Pre-bokeh pass, also used to resize the color texture.
         * @param fbo The framebuffer used for this particular effect.
         * @param color_texture The scene color texture collected from the framebuffer.
         */
        void preFilter(framebuffer& fbo, texture_handle& color_texture);
        /**
         * @brief bokeh The bokeh blur pass, blurs all everything that is outside focus range.
         * @param fbo The framebuffer used for this particular effect.
         * @param color_texture The scene color texture collected from the framebuffer.
         */
        void bokeh(framebuffer& fbo, texture_handle& color_texture);
        /**
         * @brief postFilter The post filter pass blurs the bokeh effect to look like a more solid blur.
         * @param fbo The framebuffer used for this particular effect.
         * @param color_texture The scene color texture collected from the framebuffer.
         */
        void postFilter(framebuffer& fbo, texture_handle& color_texture);
        /**
         * @brief combine The combine pass combines the area of focus, bokeh and color texture.
         * @param fbo The framebuffer used for this particular effect.
         * @param color_texture The scene color texture collected from the framebuffer.
         */
        void combine(framebuffer& fbo, texture_handle& color_texture);
        /**
         * @brief renderResults This pass is to properly display the right texture at the end.
         * @param fbo The framebuffer used for this particular effect.
         * @param color_texture The scene color texture collected from the framebuffer.
         */
        void renderResults(framebuffer& fbo, texture_handle& texture, texture_handle& color_texture);
        /**
         * @brief getTextures Gets all the necessary textures from the framebuffer.
         * @param fbo The framebuffer used for this particular effect.
         * @return Returns a tuple of a bool, stating if it managed to get the textures and the other 2 are getting the color and position texture.
         */
        std::tuple<bool, texture_handle, texture_handle> getTextures(framebuffer& fbo);
    };
}

