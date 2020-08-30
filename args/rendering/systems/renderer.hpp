#pragma once
#include <rendering/data/mesh.hpp>
#include <rendering/components/renderable.hpp>

namespace args::rendering
{
    class Renderer final : public System<Renderer>
    {
        virtual void setup()
        {

        }

        void render(time::time_span<fast_time> deltaTime)
        {
            (void)deltaTime;

            static auto query = createQuery<renderable, position, rotation, scale>();
        }
    };
}
