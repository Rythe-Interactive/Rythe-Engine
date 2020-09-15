#pragma once

#ifndef ARGS_IMPORT
#define ARGS_IMPORT
#include <core/core.hpp>
#include <core/platform/args_library.hpp>
#else
#include <core/core.hpp>
#endif // !ARGS_IMPORT

#include <physics/rigidbody.hpp>
#include <physics/data/physics_manifold_precursor.h>


namespace args::physics
{
    class PhysicsSystem final : public System<PhysicsSystem>
    {
    public:

        ecs::EntityQuery  rigidbodyIntegrationQuery;

        virtual void setup()
        {
            createProcess<&PhysicsSystem::fixedUpdate>("Physics", m_timeStep);

            rigidbodyIntegrationQuery = createQuery<rigidbody,position,rotation>();

            OptimizeBroadPhase = std::bind(&PhysicsSystem::bruteForceBroadPhase, this, std::placeholders::_1, std::placeholders::_2);
            
        }

        void fixedUpdate(time::time_span<fast_time> deltaTime)
        {
            runPhysicsPipeline();

            integrateRigidbodies(deltaTime);
        }

    private:

        std::function<void(const std::vector<physics_manifold_precursor>&, std::vector<std::vector<physics_manifold_precursor>>&)> OptimizeBroadPhase;
        const float m_timeStep = 0.02f;

        /** @brief Performs the entire physics pipeline (
         * Broadphase Collision Detection, Narrowphase Collision Detection, and the Collision Resolution)
        */
        void runPhysicsPipeline()
        {
            //-------------------------------------------------Broadphase Optimization-----------------------------------------------//
            int initialColliderID = 0;

            //recursively get all physics components from the world
            std::vector<physics_manifold_precursor> manifoldPrecursors;
            recursiveRetrievePreManifoldData(manifoldPrecursors, ecs::entity_handle(world_entity_id, m_ecs), math::mat4(1.0f), initialColliderID);

            //log::debug(" manifold precursor {}", manifoldPrecursors.size());

            std::vector<std::vector<physics_manifold_precursor>> manifoldPrecursorGrouping;
            OptimizeBroadPhase(manifoldPrecursors, manifoldPrecursorGrouping);
            
            //------------------------------------------------------ Narrophase -----------------------------------------------------//

            //-------------------------------------------------- Collision Solver ---------------------------------------------------//
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

                integrateRigidbody(rbPosHandle, rbRotHandle, rbPos, deltaTime);
            }
        }

        /**@brief recursively goes through the world to retrieve the physicsComponent of entities that have one 
        * @param [out] manifoldPrecursors A std::vector that will store the created physics_manifold_precursor from the scene graph iteration
        * @param initialEntity The entity where you would like to start the retrieval. If you would like to iterate through the entire scene
        * put the world as a parameter
        * @param parentTransform The world transform of the initialEntity. If 'initialEntity' is the world parentTransform would be the identity matrix
        * @param id An integer that is used to identiy a physics_manifold_precursor from one another
        */
        void recursiveRetrievePreManifoldData(std::vector<physics_manifold_precursor> & manifoldPrecursors, const ecs::entity_handle& initialEntity,math::mat4 parentTransform = math::mat4(1.0f),int id =0)
        {
            math::mat4 rootTransform = parentTransform;
            
            auto rotationHandle = initialEntity.get_component_handle<rotation>();
            auto positionHandle = initialEntity.get_component_handle<position>();
            auto scaleHandle = initialEntity.get_component_handle<scale>();
            auto physicsComponentHandle = initialEntity.get_component_handle<physicsComponent>();

                
            bool hasTransform = rotationHandle && positionHandle && scaleHandle;
            bool hasNecessaryComponentsForPhysicsManifold = hasTransform && physicsComponentHandle;

            int colliderID = id;
           /* log::debug("----------- recursiveRetrievePreManifoldData ------------------------");
            log::debug("colliderID count {0} ", colliderID);

            log::debug("colliderID hasTransform {0} ", hasTransform);

            log::debug("colliderID hasNecessaryComponentsForPhysicsManifold {0} ", hasNecessaryComponentsForPhysicsManifold);*/

            //if the entity has a physicsComponent and a transform and a physicsComponent
            if (hasNecessaryComponentsForPhysicsManifold)
            {
                rotation rot = rotationHandle.read();
                position pos = positionHandle.read();
                scale scale = scaleHandle.read();

                //assemble the local transform matrix of the entity
                math::mat4 scaleMat4 =
                    math::mat4
                    (scale.x, 0, 0, 0,
                        0, scale.y, 0, 0,
                        0, 0, scale.z, 0,
                        0, 0, 0, 1);

                math::mat4 rotationMat4 = math::toMat4(rot);

                math::mat4 positionMat4 =
                    math::mat4
                    (1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        pos.x, pos.y, pos.z, 1);
 
                math::mat4 localTransform = scaleMat4 * rotationMat4 * positionMat4;

                //multiply it with the parent to get the world transform
                rootTransform = parentTransform * localTransform;

                //create its physics_manifold_precursor
                physics_manifold_precursor manifoldPrecursor(rootTransform,physicsComponentHandle, colliderID);

                manifoldPrecursors.push_back(manifoldPrecursor);
            }

            //log::debug("initialEntity.child_count() {} ", initialEntity.child_count());
            //call recursiveRetrievePreManifoldData on its children
            for (int i = 0; i < initialEntity.child_count(); i++)
            {
                colliderID++;
                auto child = initialEntity.get_child(i);
                recursiveRetrievePreManifoldData(manifoldPrecursors, child, rootTransform, colliderID);
            }
                
            
        }

        /**@brief moves all physics_manifold_precursor into a singular std::vector. This esentially means that no optimization was done.
        * @note This should only be used for testing/debugging purposes
        */
        void bruteForceBroadPhase(const std::vector<physics_manifold_precursor>& manifoldPrecursors, std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping)
        {
            manifoldPrecursorGrouping.push_back(std::move(manifoldPrecursors));
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
