#pragma once
#include <core/core.hpp>

#include "../data/explode_event.hpp"
#include "../systems/explosion_particlesystem.hpp"

namespace ext {

    using namespace legion;
    class ParticleSpawner : public System<ParticleSpawner> {
        rendering::material_handle vertexColor;
        rendering::model_handle explosionSphere;
        rendering::model_handle particleSphere;

        void setup() override
        {
            bindToEvent<ext::evt::explosion_event, &ParticleSpawner::onSpawnParticles>();
            vertexColor = rendering::MaterialCache::create_material("vertexColor - ParticleSpawner", fs::view("assets://shaders/vertexcolor.shs"));
            vertexColor.set_param("color",math::colors::red);
            explosionSphere = rendering::ModelCache::create_model("explosionSphere - ParticleSpawner", fs::view("assets://models/explosionMesh.obj"));
            particleSphere = rendering::ModelCache::create_model("particleSphere - ParticleSpawner", fs::view("assets://models/particleModel.obj"));

        }

        void onSpawnParticles(evt::explosion_event* ev)
        {
            static auto query = createQuery<evt::explosion_receiver>();

            ecs::entity_handle handle;
            query.queryEntities();
            for (ecs::entity_handle h : query)
            {
                if (ev->getAffectedName() == h.read_component<evt::explosion_receiver>().name)
                {
                    handle = h;
                }
            }

            explosionParameters explosionParams{
                 math::vec3(0.07f),
                 vertexColor,
                 particleSphere,
                 3.0f,
                 math::vec3(0.2f),
                 0.99f,
                 0.99f,
                 math::colors::yellow,
                 explosionSphere
            };

            auto pSystem = rendering::ParticleSystemCache::createParticleSystem<ExplosionParticleSystem>("Exploder - ParticleSpawner", explosionParams);

            const auto [pos, rot, _] = handle.get_component_handles<transform>();
            auto ent = createEntity();

            ent.add_components<transform>(pos.read(), rot.read(), scale(1.5f));
            auto emitter = ent.add_component<rendering::particle_emitter>();
            emitter.read_modify_write([&](rendering::particle_emitter& e)
                {
                    e.playAnimation = true;
                    e.particleSystemHandle = pSystem;
                });
        }
    };
}
