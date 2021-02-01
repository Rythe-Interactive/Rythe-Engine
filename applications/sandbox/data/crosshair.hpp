#pragma once
#include <rendering/rendering.hpp>

using namespace legion;

class Crosshair : public gfx::PostProcessingEffect<Crosshair>
{
private:
    //Shaders needed.
    gfx::shader_handle m_crosshairShader;
    static gfx::texture_handle m_crosshairTexture;
    static math::vec2 m_crosshairScale;

public:
    static void setTexture(gfx::texture_handle texture);
    static void setScale(math::vec2 scale);

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
    void renderPass(gfx::framebuffer& fbo, gfx::RenderPipelineBase* pipeline, gfx::camera& cam, const gfx::camera::camera_input& camInput, time::span deltaTime);
};

