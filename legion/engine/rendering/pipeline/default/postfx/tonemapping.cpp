#include <rendering/pipeline/default/postfx/tonemapping.hpp>

namespace legion::rendering
{
    std::atomic<id_type> Tonemapping::m_currentShader = { nameHash("aces tonemapping") };

    void Tonemapping::setAlgorithm(tonemapping_type type)
    {
        static id_type acesId = nameHash("aces tonemapping");
        static id_type reinhardId = nameHash("reinhard tonemapping");
        static id_type reinhardJodieId = nameHash("reinhard jodie tonemapping");
        static id_type legionId = nameHash("legion tonemapping");
        static id_type unreal3Id = nameHash("unreal3 tonemapping");

        switch (type)
        {
        case tonemapping_type::aces:
            m_currentShader.store(acesId, std::memory_order_relaxed);
            break;
        case tonemapping_type::reinhard:
            m_currentShader.store(reinhardId, std::memory_order_relaxed);
            break;
        case tonemapping_type::reinhard_jodie:
            m_currentShader.store(reinhardJodieId, std::memory_order_relaxed);
            break;
        case tonemapping_type::legion:
            m_currentShader.store(legionId, std::memory_order_relaxed);
            break;
        case tonemapping_type::unreal3:
            m_currentShader.store(unreal3Id, std::memory_order_relaxed);
            break;
        default:
            m_currentShader.store(legionId, std::memory_order_relaxed);
            break;
        }
    }

    void Tonemapping::setup(app::window& context)
    {
        using namespace legion::core::fs::literals;
        rendering::ShaderCache::create_shader("aces tonemapping", "engine://shaders/aces.shs"_view);
        rendering::ShaderCache::create_shader("reinhard tonemapping", "engine://shaders/reinhard.shs"_view);
        rendering::ShaderCache::create_shader("reinhard jodie tonemapping", "engine://shaders/reinhardjodie.shs"_view);
        rendering::ShaderCache::create_shader("legion tonemapping", "engine://shaders/legiontonemap.shs"_view);
        rendering::ShaderCache::create_shader("unreal3 tonemapping", "engine://shaders/unreal3.shs"_view);
        addRenderPass<&Tonemapping::renderPass>();
        gamma = 2.2f;
    }

    void Tonemapping::renderPass(framebuffer& fbo, texture_handle colortexture, texture_handle depthtexture, time::span deltaTime)
    {
        static id_type gammaId = nameHash("gamma");

        auto shader = ShaderCache::get_handle(m_currentShader.load(std::memory_order_relaxed));

        fbo.bind();
        shader.bind();
        shader.get_uniform<float>(gammaId).set_value(gamma);
        shader.get_uniform_with_location<texture_handle>(SV_SCENECOLOR).set_value(colortexture);
        renderQuad();
        shader.release();
        fbo.release();

        auto tex = std::get<texture_handle>(fbo.getAttachment(GL_COLOR_ATTACHMENT0)).get_texture();
        auto size = tex.size();

        size_type maxMip = math::floor(math::log2(math::max(size.x, size.y)));

        glGenerateTextureMipmap(tex.textureId);

        std::vector<math::color> colors;
        colors.resize(1);

        glBindTexture(static_cast<GLenum>(tex.type), tex.textureId);
        glGetTexImage(static_cast<GLenum>(tex.type), maxMip, components_to_format[static_cast<int>(tex.channels)], GL_FLOAT, colors.data());
        glBindTexture(static_cast<GLenum>(tex.type), 0);

        float luminance = math::dot(math::vec3(colors[0].rgb), math::vec3(0.2126f, 0.7152f, 0.0722f));

        float newGamma = math::clamp(math::lerp(4.5f, -1.f, math::clamp(1.f - math::pow(1.f - luminance, 5.f), 0.f, 1.f)), 0.1f, 4.5f);
        gamma = math::lerp(gamma, newGamma, deltaTime.seconds());
    }

}
