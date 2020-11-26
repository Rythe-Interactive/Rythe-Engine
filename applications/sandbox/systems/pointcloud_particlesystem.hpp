#pragma once
#include <rendering/data/particle_system_base.hpp>

using namespace legion;

struct pointCloudParameters
{
    math::vec3 startingSize;
    rendering::material_handle particleMaterial;
    rendering::model_handle particleModel;

    bool looping = false;
    float maxLifeTime = 0.0f;
    float startingLifeTime = 0.0f;
    uint spawnRate = 0;
    uint particleCount = 0;
    uint maxParticles = 0;
    math::vec3 startingVel = math::vec3(0);
    float sizeOverLifeTime = 0.0f;
};

class PointCloudParticleSystem : public rendering::ParticleSystemBase
{
public:
    PointCloudParticleSystem(pointCloudParameters params, const std::vector<math::vec3>& positions)
    {
        m_looping = params.looping;
        m_maxLifeTime = params.maxLifeTime;
        m_startingLifeTime = params.startingLifeTime;
        m_spawnRate = params.spawnRate;
        m_particleCount = params.particleCount;
        m_maxParticles = params.maxParticles;
        m_startingVelocity = params.startingVel;
        m_startingSize = params.startingSize;
        m_sizeOverLifetime = params.sizeOverLifeTime;
        m_particleMaterial = params.particleMaterial;
        m_particleModel = params.particleModel;
        m_positions = positions;
    }

    void setup(ecs::component_handle<rendering::particle_emitter> emitter_handle) const override
    {
        rendering::particle_emitter emitter = emitter_handle.read();
        for (auto position : m_positions)
        {
            ecs::component_handle<rendering::particle> particleComponent = checkToRecycle(emitter_handle);
            auto ent = particleComponent.entity;
            if (!ent.has_components<transform>())
                ent.add_components<transform>();
            transform trans = ent.get_component_handles<transform>();
            core::position posParticle = trans.get<core::position>().read();
            posParticle = position;
            core::position scaleParticle = trans.get<core::scale>().read();
            scaleParticle = math::vec3(m_startingSize);
            trans.get<core::position>().write(posParticle);
            trans.get<core::scale>().write(scaleParticle);
            log::debug("Entity Created");
            createParticle(particleComponent, trans);
        }
    }

    void update(std::vector<ecs::entity_handle>, ecs::component_handle<rendering::particle_emitter>) const override
    {
        
    }

private:
    std::vector<math::vec3> m_positions;
};
