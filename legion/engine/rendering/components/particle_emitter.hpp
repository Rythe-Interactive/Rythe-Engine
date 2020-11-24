#pragma once
#include <core/core.hpp>
#include <rendering/systems/particle_system_base.hpp>

namespace legion::rendering
{
    struct particle_emitter
    {
        std::vector<ecs::entity_handle> livingParticles;
        std::vector<ecs::entity_handle> deadParticles;
        ParticleSystemHandle particleSystemHandle;
    };
}
