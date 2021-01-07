#pragma once
#include <core/engine/system.hpp>
#include "../data/animation.hpp"

namespace ext
{
    using namespace legion;

    /** @brief Animates entities with an animation component
     *  @see animation
     */
    class Animator : public System<Animator>
    {
        ecs::EntityQuery query = createQuery<animation, position, rotation, scale>();

        void setup() override
        {
            createProcess<&Animator::onUpdate>("Update");
        }
        void onUpdate(time::span delta);
    };
}
