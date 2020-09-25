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
#include <physics/physics_manifold.hpp>
#include <physics/physics_contact.h>
#include <physics/physics_component.hpp>

namespace args::physics
{
    typedef std::shared_ptr<PhysicsCollider> PhysicsColliderPtr;

    class PhysicsSystem final : public System<PhysicsSystem>
    {
    public:

        ecs::EntityQuery  rigidbodyIntegrationQuery;

        virtual void setup()
        {
            createProcess<&PhysicsSystem::fixedUpdate>("Physics", m_timeStep);

            rigidbodyIntegrationQuery = createQuery<rigidbody,position,rotation>();

            auto broadPhaseLambda = [this]
            (std::vector<physics_manifold_precursor>& manifoldPrecursors
                , std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping)
            {
                bruteForceBroadPhase(manifoldPrecursors, manifoldPrecursorGrouping);
            };
            
            m_optimizeBroadPhase = broadPhaseLambda;
            
        }

        void fixedUpdate(time::time_span<fast_time> deltaTime)
        {
            runPhysicsPipeline();

            integrateRigidbodies(deltaTime);
        }

        //The following function is public static so that it can be called by testSystem

        /**@brief recursively goes through the world to retrieve the physicsComponent of entities that have one
        * @param [out] manifoldPrecursors A std::vector that will store the created physics_manifold_precursor from the scene graph iteration
        * @param initialEntity The entity where you would like to start the retrieval. If you would like to iterate through the entire scene
        * put the world as a parameter
        * @param parentTransform The world transform of the initialEntity. If 'initialEntity' is the world, parentTransform would be the identity matrix
        * @param id An integer that is used to identiy a physics_manifold_precursor from one another
        */
        inline static void recursiveRetrievePreManifoldData(std::vector<physics_manifold_precursor>& manifoldPrecursors,
            const ecs::entity_handle& initialEntity, math::mat4 parentTransform = math::mat4(1.0f), int id = 0)
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

             //if the entity has a physicsComponent and a transform
            if (hasNecessaryComponentsForPhysicsManifold)
            {
                rotation rot = rotationHandle.read();
                position pos = positionHandle.read();
                scale scale = scaleHandle.read();

                //assemble the local transform matrix of the entity
                math::mat4 localTransform;
                math::compose(localTransform, scale, rot, pos);

                //multiply it with the parent to get the world transform
                rootTransform = parentTransform * localTransform;

                //create its physics_manifold_precursor
                physics_manifold_precursor manifoldPrecursor(rootTransform, physicsComponentHandle, colliderID);

                auto physicsComponent = physicsComponentHandle.read();

                for (auto physCollider : *physicsComponent.colliders)
                {
                    physCollider->DrawColliderRepresentation(rootTransform);
                }

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


    private:

        args::delegate<void(std::vector<physics_manifold_precursor>&, std::vector<std::vector<physics_manifold_precursor>>&)> m_optimizeBroadPhase;
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
            m_optimizeBroadPhase(manifoldPrecursors, manifoldPrecursorGrouping);
            
            //------------------------------------------------------ Narrowphase -----------------------------------------------------//
            std::vector<physics_manifold> manifoldsToSolve;

            log::debug("------------- Narrowphase");

            for (auto& manifoldPrecursors : manifoldPrecursorGrouping)
            {
                for (int i = 0; i < manifoldPrecursors.size()-1; i++)
                {
                    for (int j = i+1; j < manifoldPrecursors.size(); j++)
                    {
                        physics_manifold_precursor& precursorA = manifoldPrecursors.at(i);
                        physics_manifold_precursor& precursorB = manifoldPrecursors.at(j);

                        auto phyCompHandleA = precursorA.physicsComponentHandle;
                        auto phyCompHandleB = precursorB.physicsComponentHandle;

                        physicsComponent precursorPhyCompA = phyCompHandleA.read();
                        physicsComponent precursorPhyCompB = phyCompHandleB.read();

                        auto precursorRigidbodyA = phyCompHandleA.entity.get_component_handle<rigidbody>();
                        auto precursorRigidbodyB = phyCompHandleB.entity.get_component_handle<rigidbody>();

                        //only construct a manifold if at least one of these requirement are fulfilled
                        //1. One of the physicsComponents is a trigger and the other one is not
                        //2. One of the physicsComponent's entity has a rigidbody and the other one is not a trigger
                        //3. Both have a rigidbody

                        bool isBetweenTriggerAndNonTrigger =
                            (precursorPhyCompA.isTrigger && !precursorPhyCompB.isTrigger) || (!precursorPhyCompA.isTrigger && precursorPhyCompB.isTrigger);
                            
                        bool isBetweenRigidbodyAndNonTrigger =
                            (precursorRigidbodyA && !precursorPhyCompB.isTrigger) || (precursorRigidbodyB && !precursorPhyCompA.isTrigger);

                        bool isBetween2Rigidbodies = (precursorRigidbodyA && precursorRigidbodyB);

                        
                        if (isBetweenTriggerAndNonTrigger || isBetweenRigidbodyAndNonTrigger || isBetween2Rigidbodies)
                        {

                            constructManifoldsWithPrecursors(manifoldPrecursors.at(i), manifoldPrecursors.at(j),
                                manifoldsToSolve,
                                precursorRigidbodyA || precursorRigidbodyB
                                , precursorPhyCompA.isTrigger || precursorPhyCompB.isTrigger);
                        }
                        

                        //

                    }
                }
            }

            //-------------------------------------------------- Collision Solver ---------------------------------------------------//

            //the effective mass remains the same for every iteration of the solver. This means that we can precalculate it before
            //we start the solver

            for (auto& manifold : manifoldsToSolve)
            {
                for (auto& contact : manifold.contacts)
                {
                    contact.preCalculateEffectiveMass();
                }
            }

            //for both contact and friction resolution, an iterative algorithm is used.
            //Everytime physics_contact::resolveContactConstraint is called, the rigidbodies in question get closer to the actual
            //"correct" linear and angular velocity (Projected Gauss Seidel). For the sake of simplicity, an arbitrary number is set for the
            //iteration count.

            //resolve contact constraint
            for (size_t i = 0; i < constants::contactSolverIterationCount; i++)
            {
                for (auto& manifold : manifoldsToSolve)
                {
                    for (auto& contact : manifold.contacts)
                    {
                        contact.resolveContactConstraint();
                    }
                }
            }

            //resolve friction constraint
            for (size_t i = 0; i < constants::frictionSolverIterationCount; i++)
            {
                for (auto& manifold : manifoldsToSolve)
                {
                    for (auto& contact : manifold.contacts)
                    {
                        contact.resolveFrictionConstraint();
                    }
                }
            }


        }


       
        /**@brief moves all physics_manifold_precursor into a singular std::vector. This esentially means that no optimization was done.
        * @note This should only be used for testing/debugging purposes
        */
        void bruteForceBroadPhase(std::vector<physics_manifold_precursor>& manifoldPrecursors,
            std::vector<std::vector<physics_manifold_precursor>>& manifoldPrecursorGrouping)
        {
            manifoldPrecursorGrouping.push_back(std::move(manifoldPrecursors));
        }

        /**@brief given 2 physics_manifold_precursors precursorA and precursorB, create a manifold for each collider in precursorA 
        * with every other collider in precursorB. The manifolds that involve rigidbodies are then pushed into the given manifold list
        * @param manifoldsToSolve [out] a std::vector of physics_manifold that will store the manifolds created
        * @param isRigidbodyInvolved A bool that indicates whether a rigidbody is involved in this manifold
        * @param isTriggerInvolved A bool that indicates whether a physicsComponent with a physicsComponent::isTrigger set to true is involved in this manifold
        */
        void constructManifoldsWithPrecursors(physics_manifold_precursor& precursorA, physics_manifold_precursor& precursorB,
            std::vector<physics_manifold>& manifoldsToSolve,bool isRigidbodyInvolved,bool isTriggerInvolved)
        {
            

            auto physicsComponentA = precursorA.physicsComponentHandle.read();
            auto physicsComponentB = precursorB.physicsComponentHandle.read();

            for (auto colliderA : *physicsComponentA.colliders)
            {
                for (auto colliderB : *physicsComponentB.colliders)
                {
                    physics::physics_manifold m;
                    constructManifoldWithCollider(colliderA,colliderB,precursorA,precursorB,m);

                    if (isRigidbodyInvolved)
                    {
                        //send it to 'manifoldsToSolve'
                    }

                    if (isTriggerInvolved)
                    {
                        //notify both the trigger and triggerer
                    }

                }
            }
        }

        void constructManifoldWithCollider(
            PhysicsColliderPtr colliderA, PhysicsColliderPtr colliderB
            , physics_manifold_precursor& precursorA, physics_manifold_precursor& precursorB,physics_manifold& manifold)
        {
            manifold.colliderA = colliderA;
            manifold.colliderB = colliderB;

            manifold.physicsCompA = precursorA.physicsComponentHandle;
            manifold.physicsCompB = precursorB.physicsComponentHandle;

            manifold.transformA = precursorA.worldTransform;
            manifold.transformB = precursorB.worldTransform;

           // log::debug("colliderA->CheckCollision(colliderB, manifold)");
            colliderA->CheckCollision(colliderB, manifold);

        }

        /** @brief gets all the entities with a rigidbody component and calls the integrate function on them
        */
        void integrateRigidbodies(float deltaTime)
        {
            for (auto ent : rigidbodyIntegrationQuery)
            {
                auto rbPosHandle = ent.get_component_handle<position>();
                auto rbRotHandle = ent.get_component_handle<rotation>();
                auto rbRigidbodyHandle = ent.get_component_handle<rigidbody>();

                integrateRigidbody(rbPosHandle, rbRotHandle, rbRigidbodyHandle, deltaTime);
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

            //-------------------- update position ------------------//
            math::vec3 acc = rb.forceAccumulator * rb.inverseMass;
            rb.velocity += (acc + constants::gravity) * dt;
            rbPos += rb.velocity * dt;

            //-------------------- update rotation ------------------//
            math::vec3 angularAcc = rb.torqueAccumulator * rb.inverseInertiaTensor;
            rb.angularVelocity += (angularAcc);

            //construct the rotation by using the direction of angularVelocity as the axis and its length as the angle
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
