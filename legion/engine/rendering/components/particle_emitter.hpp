#pragma once
#include <core/core.hpp>
#include <rendering/data/particle_system_cache.hpp>
#include <rendering/data/Octree.hpp>
namespace legion::rendering
{
    /**
     * @brief Particle Emitter is the component that hold all the particles and related particle system.
     */
    struct particle_emitter
    {
        //std::unordered_set<ecs::entity_handle> livingParticleSet
       // std::dubleEndedq;
        //std::vector<ecs::entity_handle> livingParticlesVector;
        std::vector<ecs::entity_handle> livingParticles;
        std::vector<ecs::entity_handle> deadParticles;
        bool playAnimation = false;
        ParticleSystemHandle particleSystemHandle;
        bool setupCompleted;

        int CurrentLOD = 0;
        rendering::Octree<uint8>* Tree;
        std::vector<int> ElementsPerLOD;
    };
}
