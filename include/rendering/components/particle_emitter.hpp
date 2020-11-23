#pragma once
#include <core/core.hpp>

namespace legion::rendering
{
    struct particle_emitter
    {
        std::vector<ecs::entity_handle> livingParticles;
        std::vector<ecs::entity_handle> deadParticles;
    };
}
