#include <rendering/systems/renderer.hpp>

namespace legion::rendering
{
    void Renderer::setup()
    {
        createProcess<&Renderer::render>("Rendering");
    }
}
