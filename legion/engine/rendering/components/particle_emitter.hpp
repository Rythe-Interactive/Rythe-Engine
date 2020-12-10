#pragma once
#include <core/core.hpp>
#include <rendering/data/particle_system_cache.hpp>

namespace legion::rendering
{
    /**
     * @brief Particle Emitter is the component that hold all the particles and related particle system.
     */
    struct particle_emitter
    {
        std::vector<ecs::entity_handle> livingParticles;
        std::vector<ecs::entity_handle> deadParticles;
        bool playAnimation = false;
        ParticleSystemHandle particleSystemHandle;
        bool setupCompleted;

        bool particleIsAlive(ecs::component_handle<particle> particularParticle)
        {
            auto result = std::find(livingParticles.begin(), livingParticles.end(), particularParticle);
            if (result == livingParticles.end()) return false;
            return true;
        }
    };
}
