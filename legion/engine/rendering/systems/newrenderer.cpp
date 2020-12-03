#include <rendering/systems/newrenderer.hpp>

namespace legion::rendering
{
    void Renderer::setup()
    {
        createProcess<&Renderer::render>("Rendering");
    }
}
