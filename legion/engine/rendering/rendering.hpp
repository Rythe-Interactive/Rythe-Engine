#pragma once
#include <rendering/data/model.hpp>
#include <rendering/systems/renderer.hpp>
#include <rendering/module/renderingmodule.hpp>
#include <rendering/debugrendering.hpp>
#include <rendering/pipeline/default/stages/postprocessingstage.hpp>
#include <rendering/pipeline/default/postfx/tonemapping.hpp>
#include <rendering/data/buffer.hpp>
#include <rendering/data/framebuffer.hpp>
#include <rendering/data/renderbuffer.hpp>
#include <rendering/data/vertexarray.hpp>
#include <rendering/data/material.hpp>
#include <rendering/data/postprocessingeffect.hpp>

namespace legion
{
    namespace gfx = ::legion::rendering;
}
