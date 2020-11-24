#include <rendering/systems/particle_system_base.hpp>
#include <rendering/components/particle_emitter.hpp>
#include <rendering/components/renderable.hpp>


namespace legion::rendering
{
    ecs::component_handle<particle> ParticleSystemBase::createParticle(ecs::component_handle<transform> transformHandle, ecs::component_handle<particle_emitter> emitterHandle)
    {
        particle_emitter emitter = emitterHandle.read();
        if (!emitter.deadParticles.empty())
        {
            ecs::entity_handle refurbishedParticle = emitter.deadParticles[emitter.deadParticles.back()];
            //auto RPtransform = refurbishedParticle.get_component_handle<transform>().read();
            //auto ;
        }
    }

    void ParticleSystemBase::cleanUpParticle(ecs::component_handle<particle> particleHandle, ecs::component_handle<particle_emitter> emitterHandle)
    {
        //Read emitter
        particle_emitter emitter = emitterHandle.read();
        const auto particle = std::find(emitter.livingParticles.begin(), emitter.livingParticles.end(), particleHandle);
        if (particle != emitter.livingParticles.end())
        {
            ecs::entity_handle particularParticle = *particle;
            //Remove from living
            const auto iterator = std::remove(emitter.livingParticles.begin(), emitter.livingParticles.end(), particularParticle);
            emitter.livingParticles.erase(iterator);
            //Add to dead
            emitter.deadParticles.emplace_back(particularParticle);
            //Remove renderable to stop them from being rendered
            particularParticle.remove_component<renderable>();
        }
        //Write to emitter
        emitterHandle.write(emitter);
    }
}
