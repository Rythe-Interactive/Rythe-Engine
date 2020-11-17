#pragma once
#include <rendering/data/model.hpp>
#include <rendering/data/shader.hpp>
#include <rendering/components/renderable.hpp>
#include <rendering/components/camera.hpp>
#include <rendering/pipeline/base/renderstage.hpp>

#include <map>
#include <memory>

namespace legion::rendering
{
    class ARGS_API Pipeline
    {
        std::map<priority_type, std::unique_ptr<RenderStage>> m_stages;
    };
}
