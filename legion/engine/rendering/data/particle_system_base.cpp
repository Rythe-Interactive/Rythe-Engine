#include <rendering/data/particle_system_base.hpp>
#include <rendering/components/particle_emitter.hpp>
#include <rendering/components/renderable.hpp>


namespace legion::rendering
{
    ecs::EcsRegistry* ParticleSystemBase::m_registry;

    void ParticleSystemBase::createParticle(ecs::component_handle<particle> particleHandle, transform transformHandle) const
    {
        ecs::entity_handle particularParticle = particleHandle.entity;

        //Handle model and material assigning.
        particularParticle.add_components<renderable>(m_particleModel.get_mesh(), mesh_renderer(m_particleMaterial));
    }

    void ParticleSystemBase::cleanUpParticle(const ecs::entity_handle& particleHandle, ecs::component_handle<particle_emitter>& emitterHandle) const
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
            particularParticle.remove_component<mesh_renderer>();
            particularParticle.remove_component<mesh_filter>();
        }
        //Write to emitter
        emitterHandle.write(emitter);
    }

    ecs::component_handle<particle> ParticleSystemBase::checkToRecycle(ecs::component_handle<particle_emitter> emitterHandle) const
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
        emitterHandle.write(emitter);
        return particularParticle.get_component_handle<particle>();
    }
}
