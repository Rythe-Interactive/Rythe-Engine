#pragma once
#include <core/core.hpp>
#include <rendering/data/particle_system_base.hpp>

namespace legion::rendering
{
    class ParticleSystemManager : public System<ParticleSystemManager>
    {
        void setup()
        {
            createProcess<&ParticleSystemManager::update>("Update");
        }

        void update(time::span deltaTime)
        {
            
        }
    };
}
