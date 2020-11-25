#include <rendering/data/particle_system_base.hpp>
#include <rendering/components/particle_emitter.hpp>
#include <rendering/components/renderable.hpp>


namespace legion::rendering
{
    void ParticleSystemBase::createParticle(ecs::component_handle<particle> particleHandle, ecs::component_handle<transform> transformHandle, ecs::component_handle<particle_emitter> emitterHandle)
    {
        ecs::entity_handle particularParticle = particleHandle.entity;

        //Handle transform adjustments.
        particularParticle.add_component(transformHandle);

        //Handle model and material assigning.
        ecs::component_handle<renderable> renderableHandle = particularParticle.add_component<renderable>();
        renderable renderable = renderableHandle.read();
        renderable.model = m_particleModel;
        renderable.material = m_particleMaterial;
        renderableHandle.write(renderable);
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
            particularParticle.remove_component<transform>();
        }
        //Write to emitter
        emitterHandle.write(emitter);
    }

    ecs::component_handle<particle> ParticleSystemBase::checkToRecycle(ecs::component_handle<particle_emitter> emitterHandle)
    {
        particle_emitter emitter = emitterHandle.read();
        ecs::entity_handle particularParticle;

        if (!emitter.deadParticles.empty())
        {
            //Get particle from dead particle list.
            particularParticle = emitter.deadParticles[emitter.deadParticles.back()];
            //Remove particle from dead particle list.
            const auto iterator = std::remove(emitter.deadParticles.begin(), emitter.deadParticles.end(), particularParticle);
            emitter.deadParticles.erase(iterator);
            //Add particle to living particle list.
            emitter.livingParticles.emplace_back(particularParticle);
        }
        else
        {
            //Create new particle entity.
            particularParticle = m_registry->createEntity();
            particularParticle.add_component<particle>();

            //Add new particle to living particle list.
            emitter.livingParticles.emplace_back(particularParticle);
        }
        return particularParticle.get_component_handle<particle>();
    }
}
