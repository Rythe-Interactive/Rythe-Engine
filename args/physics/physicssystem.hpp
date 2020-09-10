#pragma once

#ifndef ARGS_IMPORT
#define ARGS_IMPORT
#include <core/core.hpp>
#include <application/application.hpp>
#include <core/platform/args_library.hpp>
#else
#include <core/core.hpp>
#endif // !ARGS_IMPORT

#include <physics/rigidbody.hpp>

namespace args::physics
{
    class PhysicsSystem final : public System<PhysicsSystem>
    {
    public:

        ecs::EntityQuery  rigidbodyIntegrationQuery;

        virtual void setup()
        {
            createProcess<&PhysicsSystem::fixedUpdate>("Physics", m_timeStep);

            rigidbodyIntegrationQuery = createQuery<rigidbody,position,rotation,physicsComponent>();
            
        }

        void fixedUpdate(time::time_span<fast_time> deltaTime)
        {
            runPhysicsPipeline();

            integrateRigidbodies(deltaTime);
        }

    private:

        const float m_timeStep = 0.02f;

        /** @brief Performs the entire physics pipeline (
         * Broadphase Collision Detection, Narrowphase Collision Detection, and the Collision Resolution)
        */
        void runPhysicsPipeline()
        {
            //Broadphase Optimization

            //Narrophase 

            //Collision Resolution

        }

        /** @brief gets all the entities with a rigidbody component and calls the integrate function on them
        */
        void integrateRigidbodies(float deltaTime)
        {
            for (auto ent : rigidbodyIntegrationQuery)
            {
                auto rbPosHandle = ent.get_component_handle<position>();
                auto rbRotHandle = ent.get_component_handle<rotation>();
                auto rbPos = ent.get_component_handle<rigidbody>();

                integrateRigidbody(rbPosHandle, rbRotHandle, rbPos,deltaTime);
            }
        }

        /** @brief given a set of component handles, updates the position and orientation of an entity with a rigidbody component.
        */
        void integrateRigidbody(ecs::component_handle<position>& posHandle
            , ecs::component_handle<rotation>& rotHandle , ecs::component_handle<rigidbody>& rbHandle,float dt)
        {
            auto rb = rbHandle.read();
            auto rbPos = posHandle.read();
            auto rbRot = rotHandle.read();

            math::vec3 acc = rb.forceAccumulator * rb.inverseMass;
            rb.velocity += (acc + constants::gravity) * dt;
            rbPos += rb.velocity * dt;

            math::vec3 angularAcc = rb.torqueAccumulator * rb.inverseInertiaTensor;
            rb.angularVelocity += (angularAcc);

            float dtAngle = math::length(rb.angularVelocity) * dt;
            rbRot *= math::angleAxis(math::deg2rad(dtAngle), math::normalize(rb.angularVelocity));

            rb.resetAccumulators();

            //for now assume that there is no offset from bodyP
            rb.globalCentreOfMass = rbPos;

            rbHandle.write(rb);
            posHandle.write(rbPos);
            rotHandle.write(rbRot);

        }


    };
}
