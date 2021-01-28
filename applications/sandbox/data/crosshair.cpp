#include "crosshair.hpp"

gfx::texture_handle Crosshair::m_crosshairTexture;
math::vec2 Crosshair::m_crosshairScale = math::vec2(1.f, 1.f);

void Crosshair::setTexture(gfx::texture_handle texture)
{
    m_crosshairTexture = texture;
}

void Crosshair::setScale(math::vec2 scale)
{
    m_crosshairScale = scale;
}

void Crosshair::setup(app::window& context)
{
    using namespace fs::literals;
    m_crosshairShader = gfx::ShaderCache::create_shader("crosshair shader", "assets://shaders/crosshair.shs"_view);
    addRenderPass<&Crosshair::renderPass>();
}

void Crosshair::renderPass(gfx::framebuffer& fbo, gfx::RenderPipelineBase* pipeline, gfx::camera& cam, const gfx::camera::camera_input& camInput, time::span deltaTime)
{
    auto color_attachment = fbo.getAttachment(FRAGMENT_ATTACHMENT);
    if (!std::holds_alternative<gfx::texture_handle>(color_attachment))
        return;

    auto color_texture = std::get<gfx::texture_handle>(color_attachment);

    fbo.bind();
    m_crosshairShader.bind();
    m_crosshairShader.get_uniform<math::vec2>("crosshairScale").set_value(m_crosshairScale);
    m_crosshairShader.get_uniform<gfx::texture_handle>("crosshairTex").set_value(m_crosshairTexture);
    m_crosshairShader.get_uniform_with_location<gfx::texture_handle>(SV_SCENECOLOR).set_value(color_texture);
    renderQuad();
    m_crosshairShader.release();
    fbo.release();
}
