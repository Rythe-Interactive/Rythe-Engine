#pragma once
#include <rendering/pipeline/base/renderstage.hpp>
#include <rendering/pipeline/base/pipeline.hpp>
#include <rendering/data/vertexarray.hpp>
#include <rendering/data/buffer.hpp>

namespace legion::rendering
{

    class PostProcessingStage : public RenderStage<PostProcessingStage>
    {
    private:
        static std::multimap<priority_type, shader_handle> m_shaders;
        vertexarray m_quadVAO;
        buffer m_quadVBO;
    public:
        static void addShader(shader_handle shaderToAdd, priority_type priority = default_priority);
        static void removeShader(shader_handle shaderToRemove);

        virtual void setup(app::window& context) override;
        virtual void render(app::window& context, camera& cam, const camera::camera_input& camInput, time::span deltaTime) override;
        virtual priority_type priority() override;
    };

}

