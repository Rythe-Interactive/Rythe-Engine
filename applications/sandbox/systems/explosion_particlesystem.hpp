#pragma once
#include <rendering/data/particle_system_base.hpp>

#include <rendering/systems/renderer.hpp>

using namespace legion;
/**
 * @struct explosionParameters
 * @brief A struct that simplifies the parameter input of the explosion particle system constructor.
 */
struct explosionParameters
{
    math::vec3 startingSize;
    rendering::material_handle particleMaterial;
    rendering::model_handle particleModel;
    float maxLifeTime;
    math::vec3 startingVel;
    float decelerationScalar;
    float sizeOverLifeTime;
    math::color endColor;
    rendering::model_handle explosionModel;

    float startingLifeTime = 0.0f;
    bool looping = false;
    uint spawnRate = 0;
    uint particleCount = 0;
    uint maxParticles = 0;
};

/**
 * @struct ExplosionParticleSystem
 * @brief Explosion particle system that can be used for explosion VFX
 */
class ExplosionParticleSystem : public rendering::ParticleSystemBase
{
public:
    /**
     * @brief Constructor used to set all the member variables.
     */
    ExplosionParticleSystem(explosionParameters params)
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
        m_explosionModel = params.explosionModel;

        m_decelerationScalar = params.decelerationScalar;
        m_endColor = params.endColor;
        m_beginColor = m_particleMaterial.get_param<math::vec4>("color");
    }
    /**
     * @brief The setup function used to initialize all the particles.
	 */
    void setup(ecs::component_handle<rendering::particle_emitter> emitter_handle) const override
    {
        auto vertPositions = m_explosionModel.get_mesh().get().second.vertices;

        //emitter positions/origin
        const auto emitterPosHandle = emitter_handle.entity.get_component_handle<position>();
        auto scaleOfEmitter = emitter_handle.entity.get_component_handle<scale>().read();
        const math::vec3 emitterPos = emitterPosHandle.read();

        auto emitter = emitter_handle.read();
        //Loop through each vertex in the model.
        for (math::vec3 vert_position : vertPositions)
        {
            #pragma region Create particle
            //Checks the emitter if it has a recycled particle to use, if not it creates a new one.
            ecs::component_handle<rendering::particle> particleComponent = checkToRecycle(emitter_handle);

            auto ent = particleComponent.entity;
            //Checks if the entity has a transform, if not it adds one.
            if (!ent.has_components<transform>())
                ent.add_components<transform>();

            //Gets position, rotation and scale of entity.
            auto trans = ent.get_component_handles<transform>();
            auto& [pos, _, scale] = trans;

            //Sets the particle scale to the right scale.
            auto newPos = emitterPos;
            pos.write(newPos);
            scale.write(math::vec3(m_startingSize)* scaleOfEmitter);

            //Populates the particle with the appropriate stuffs.
            createParticle(particleComponent, trans);
            #pragma endregion 
            #pragma region Set directions
            //Calculates position offset from center to create a direction for the velocity.
            math::vec3 pointPos = emitterPos + vert_position;
            math::vec3 pointDirection = math::normalize(pointPos - emitterPos);
            #pragma endregion
            #pragma region Set parameter values
            //Read particle component to set its lifetime and its velocity.
            rendering::particle particularParticle = particleComponent.read();
            particularParticle.lifeTime = 0;
            particularParticle.particleVelocity = m_startingVelocity.x * pointDirection * scaleOfEmitter.r;
            particleComponent.write(particularParticle);
#           pragma endregion
        }
    }

    /**
     * @brief The update function that loops through all particles to update their positions, scale and color.
     */
    void update(std::vector<ecs::entity_handle> particle_list, ecs::component_handle<rendering::particle_emitter> particle_emitter, time::span delta_time) const override
    {
        //Read the scale of the emitter.
        auto scaleOfEmitter = particle_emitter.entity.get_component_handle<scale>().read();
        //Read the emitter component.
        auto emitter = particle_emitter.read();
        //Checks if the emitter is supposed to start animating.
        if (emitter.playAnimation)
        {
            //Loops through all the particles to update them.
            for (int i = 0; i < particle_list.size(); i++)
            {
                //Get the specified particle entity.
                auto particleEnt = particle_list[i];

                //Get the specified particle handle.
                auto particleHandle = particleEnt.get_component_handle<rendering::particle>();
                rendering::particle particle = particleHandle.read();

                //Checks if the particle lifetime has surpassed the total lifetime. If so, it gets recycled.
                if (particle.lifeTime >= m_maxLifeTime* scaleOfEmitter.r)
                {
                    #pragma region Check if still alive
                    cleanUpParticle(particleEnt, particle_emitter);
                    #pragma endregion 
                }
                else
                {
                    //Get transform
                    auto [position, _, scale] = particleEnt.get_component_handles<transform>();
                    //Update position
                    math::vec3 pos = position.read();
                    pos.x += particle.particleVelocity.x * delta_time;
                    pos.y += particle.particleVelocity.y * delta_time;
                    pos.z += particle.particleVelocity.z * delta_time;
                    position.write(pos);
                    //Update size
                    math::vec3 size = scale.read();
                    size *= m_sizeOverLifetime;
                    scale.write(size);
                    //Update velocity
                    particle.particleVelocity *= m_decelerationScalar;

                    //check if first particle, because material is universal. dont wanna set it every frame
                    if (i == 0)
                    {
                        //Update color
                        auto meshRenderer = particleEnt.get_component_handle<rendering::mesh_renderer>().read();
                        rendering::material_handle matHandle = meshRenderer.material;
                        math::color rgba = math::lerp(m_beginColor, m_endColor, (particle.lifeTime / m_maxLifeTime));
                        matHandle.set_param("color", rgba);
                    }

                    //update lifetime
                    particle.lifeTime += delta_time;

                    particleHandle.write(particle);
                }
            }
            //If the list is empty, It will destroy the emitter.
            if (particle_list.size() <= 0)
            {
                particle_emitter.destroy();
            }
        }
    }

private:
    static const int FindIndexOf(std::vector<ecs::entity_handle> list, ecs::entity_handle entity)
    {
        for (int i = 0; i < list.size(); i++)
        {
            if (list[i] == entity) return i;
        }
        return -1;
    }

    float m_decelerationScalar;
    math::color m_beginColor;
    math::color m_endColor;
    rendering::model_handle m_explosionModel;

};
