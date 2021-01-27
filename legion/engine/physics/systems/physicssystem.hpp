#pragma once
#include <core/core.hpp>
#include <physics/broadphasecollisionalgorithms/broadphasecollisionalgorithm.hpp>
#include <physics/broadphasecollisionalgorithms/broadphaseuniformgrid.hpp>
#include <physics/broadphasecollisionalgorithms/broadphasebruteforce.hpp>
#include <physics/components/rigidbody.hpp>
#include <physics/data/physics_manifold_precursor.hpp>
#include <physics/data/physics_manifold.hpp>
#include <physics/physics_contact.hpp>
#include <physics/components/physics_component.hpp>
#include <physics/data/identifier.hpp>
#include <physics/events/events.hpp>
#include <memory>
#include <rendering/debugrendering.hpp>
#include <physics/components/fracturer.hpp>

namespace legion::physics
{
    struct MeshLine
    {
        math::vec3 start;
        math::vec3 end;
        math::color Color;
    };

    class PhysicsSystem final : public System<PhysicsSystem>
    {
    public:
        static bool IsPaused;
        static bool oneTimeRunActive;

        ecs::EntityQuery manifoldPrecursorQuery;

        //TODO move implementation to a seperate cpp file

        virtual void setup()
        {
            createProcess<&PhysicsSystem::fixedUpdate>("Physics", m_timeStep);

            manifoldPrecursorQuery = createQuery<position, rotation, scale, physicsComponent>();

            m_broadPhase = std::make_unique<BroadphaseBruteforce>();
        }

        void fixedUpdate(time::time_span<fast_time> deltaTime)
        {
            static time::timer physicsTimer;
            log::debug("{}ms", physicsTimer.restart().milliseconds());
            OPTICK_EVENT();

            ecs::component_container<rigidbody> rigidbodies;
            std::vector<byte> hasRigidBodies;

            {
                OPTICK_EVENT("Fetching data");
                manifoldPrecursorQuery.queryEntities();

                rigidbodies.resize(manifoldPrecursorQuery.size());
                hasRigidBodies.resize(manifoldPrecursorQuery.size());

                m_scheduler->queueJobs(manifoldPrecursorQuery.size(), [&]() {
                    id_type index = async::this_job::get_id();
                    auto entity = manifoldPrecursorQuery[index];
                    if (entity.has_component<rigidbody>())
                    {
                        hasRigidBodies[index] = true;
                        rigidbodies[index] = entity.read_component<rigidbody>();
                    }
                    else
                        hasRigidBodies[index] = false;
                    }).wait();
            }

            auto& physComps = manifoldPrecursorQuery.get<physicsComponent>();
            auto& positions = manifoldPrecursorQuery.get<position>();
            auto& rotations = manifoldPrecursorQuery.get<rotation>();
            auto& scales = manifoldPrecursorQuery.get<scale>();

            if (!IsPaused)
            {
                integrateRigidbodies(hasRigidBodies, rigidbodies, deltaTime);
                runPhysicsPipeline(hasRigidBodies, rigidbodies, physComps, positions, rotations, scales, deltaTime);
                integrateRigidbodyQueryPositionAndRotation(hasRigidBodies, positions, rotations, rigidbodies, deltaTime);
            }

            if (oneTimeRunActive)
            {
                oneTimeRunActive = false;

                integrateRigidbodies(hasRigidBodies, rigidbodies, deltaTime);
                runPhysicsPipeline(hasRigidBodies, rigidbodies, physComps, positions, rotations, scales, deltaTime);
                integrateRigidbodyQueryPositionAndRotation(hasRigidBodies, positions, rotations, rigidbodies, deltaTime);
            }

            {
                OPTICK_EVENT("Writing data");
                m_scheduler->queueJobs(manifoldPrecursorQuery.size(), [&]() {
                    id_type index = async::this_job::get_id();
                    if (hasRigidBodies[index])
                    {
                        auto entity = manifoldPrecursorQuery[index];
                        entity.write_component(rigidbodies[index]);
                    }
                    }).wait();

                    manifoldPrecursorQuery.submit<physicsComponent>();
                    manifoldPrecursorQuery.submit<position>();
                    manifoldPrecursorQuery.submit<rotation>();
            }
        }

        void bulkRetrievePreManifoldData(
            ecs::component_container<physicsComponent>& physComps,
            ecs::component_container<position>& positions,
            ecs::component_container<rotation>& rotations,
            ecs::component_container<scale>& scales,
            std::vector<physics_manifold_precursor>& manifoldPrecursors)
        {
            OPTICK_EVENT();
            manifoldPrecursors.resize(physComps.size());

            m_scheduler->queueJobs(physComps.size(), [&]() {
                id_type index = async::this_job::get_id();
                math::mat4 transf;
                math::compose(transf, scales[index], rotations[index], positions[index]);

                for (auto& collider : physComps[index].colliders)
                    collider->UpdateTransformedTightBoundingVolume(transf);

                manifoldPrecursors[index] = { transf, &physComps[index], index, manifoldPrecursorQuery[index] };
                }).wait();
        }

        /**@brief Sets the broad phase collision detection method
         * Use BroadPhaseBruteForce to not use any broad phase collision detection
         */
        template <typename BroadPhaseType, typename ...Args>
        static void setBroadPhaseCollisionDetection(Args&& ...args)
        {
            static_assert(std::is_base_of_v<BroadPhaseCollisionAlgorithm, BroadPhaseType>, "Broadphase type did not inherit from BroadPhaseCollisionAlgorithm");
            m_broadPhase = std::make_unique<BroadPhaseType>(std::forward<Args>(args)...);
        }


    private:

        static std::unique_ptr<BroadPhaseCollisionAlgorithm> m_broadPhase;
        //legion::delegate<void(std::vector<physics_manifold_precursor>&, std::vector<std::vector<physics_manifold_precursor>>&)> m_optimizeBroadPhase;
        const float m_timeStep = 0.02f;


        math::ivec3 uniformGridCellSize = math::ivec3(1, 1, 1);

        /** @brief Performs the entire physics pipeline (
         * Broadphase Collision Detection, Narrowphase Collision Detection, and the Collision Resolution)
        */
        void runPhysicsPipeline(
            std::vector<byte>& hasRigidBodies,
            ecs::component_container<rigidbody>& rigidbodies,
            ecs::component_container<physicsComponent>& physComps,
            ecs::component_container<position>& positions,
            ecs::component_container<rotation>& rotations,
            ecs::component_container<scale>& scales,
            float deltaTime)
        {
            OPTICK_EVENT();

            //-------------------------------------------------Broadphase Optimization-----------------------------------------------//

            //recursively get all physics components from the world
            std::vector<physics_manifold_precursor> manifoldPrecursors;
            bulkRetrievePreManifoldData(physComps, positions, rotations, scales, manifoldPrecursors);

            std::vector<std::vector<physics_manifold_precursor>> manifoldPrecursorGrouping;
            //m_optimizeBroadPhase(manifoldPrecursors, manifoldPrecursorGrouping);
            m_broadPhase->collectPairs(std::move(manifoldPrecursors), manifoldPrecursorGrouping);

            //------------------------------------------------------ Narrowphase -----------------------------------------------------//
            std::vector<physics_manifold> manifoldsToSolve;

            {
                OPTICK_EVENT("Narrowphase");
                size_type totalChecks = 0;
                for (auto& manifoldPrecursor : manifoldPrecursorGrouping)
                {
                    for (int i = 0; i < manifoldPrecursor.size() - 1; i++)
                    {
                        for (int j = i + 1; j < manifoldPrecursor.size(); j++)
                        {
                            totalChecks++;
                            assert(j != manifoldPrecursor.size());

                            physics_manifold_precursor& precursorA = manifoldPrecursor.at(i);
                            physics_manifold_precursor& precursorB = manifoldPrecursor.at(j);

                            auto& precursorPhyCompA = *precursorA.physicsComp;
                            auto& precursorPhyCompB = *precursorB.physicsComp;

                            auto& precursorRigidbodyA = rigidbodies[precursorA.id];
                            auto& precursorRigidbodyB = rigidbodies[precursorB.id];

                            //only construct a manifold if at least one of these requirement are fulfilled
                            //1. One of the physicsComponents is a trigger and the other one is not
                            //2. One of the physicsComponent's entity has a rigidbody and the other one is not a trigger
                            //3. Both have a rigidbody

                            bool isBetweenTriggerAndNonTrigger =
                                (precursorPhyCompA.isTrigger && !precursorPhyCompB.isTrigger) || (!precursorPhyCompA.isTrigger && precursorPhyCompB.isTrigger);

                            bool isBetweenRigidbodyAndNonTrigger =
                                (hasRigidBodies[precursorA.id] && !precursorPhyCompB.isTrigger) || (hasRigidBodies[precursorB.id] && !precursorPhyCompA.isTrigger);

                            bool isBetween2Rigidbodies = (hasRigidBodies[precursorA.id] && hasRigidBodies[precursorB.id]);


                            if (isBetweenTriggerAndNonTrigger || isBetweenRigidbodyAndNonTrigger || isBetween2Rigidbodies)
                            {
                                constructManifoldsWithPrecursors(rigidbodies, hasRigidBodies, precursorA, precursorB,
                                    manifoldsToSolve,
                                    hasRigidBodies[precursorA.id] || hasRigidBodies[precursorB.id]
                                    , precursorPhyCompA.isTrigger || precursorPhyCompB.isTrigger);
                            }
                        }
                    }
                }
                log::debug("groupings {}", manifoldPrecursorGrouping.size());
                log::debug("total checks {}", totalChecks);
            }

            //------------------------------------------------ Pre Collision Solve Events --------------------------------------------//

            // all manifolds are initially valid

            std::vector<byte> manifoldValidity(manifoldsToSolve.size(), true);

            //TODO we are currently hard coding fracture, this should be an event at some point
            {
                OPTICK_EVENT("Fracture");
                for (size_t i = 0; i < manifoldsToSolve.size(); i++)
                {
                    auto& manifold = manifoldsToSolve.at(i);

                    auto& entityHandleA = manifold.entityA;
                    auto& entityHandleB = manifold.entityB;

                    auto fracturerHandleA = entityHandleA.get_component_handle<Fracturer>();
                    auto fracturerHandleB = entityHandleB.get_component_handle<Fracturer>();

                    bool currentManifoldValidity = manifoldValidity.at(i);

                    //log::debug("- A Fracture Check");

                    if (fracturerHandleA)
                    {
                        auto fracturerA = fracturerHandleA.read();
                        //log::debug(" A is fracturable");
                        fracturerA.HandleFracture(manifold, currentManifoldValidity, true);

                        fracturerHandleA.write(fracturerA);
                    }

                    //log::debug("- B Fracture Check");

                    if (fracturerHandleB)
                    {
                        auto fracturerB = fracturerHandleB.read();
                        //log::debug(" B is fracturable");
                        fracturerB.HandleFracture(manifold, currentManifoldValidity, false);

                        fracturerHandleB.write(fracturerB);
                    }

                    manifoldValidity.at(i) = currentManifoldValidity;
                }
            }
            //-------------------------------------------------- Collision Solver ---------------------------------------------------//
            //for both contact and friction resolution, an iterative algorithm is used.
            //Everytime physics_contact::resolveContactConstraint is called, the rigidbodies in question get closer to the actual
            //"correct" linear and angular velocity (Projected Gauss Seidel). For the sake of simplicity, an arbitrary number is set for the
            //iteration count.

            //the effective mass remains the same for every iteration of the solver. This means that we can precalculate it before
            //we start the solver

            //log::debug("--------------Logging contacts for manifold -------------------");
            {
                OPTICK_EVENT("Resolve collisions");

                initializeManifolds(manifoldsToSolve, manifoldValidity);

                {
                    OPTICK_EVENT("Resolve contact constraints");

                    //resolve contact constraint
                    for (size_t contactIter = 0;
                        contactIter < constants::contactSolverIterationCount; contactIter++)
                    {
                        resolveContactConstraint(manifoldsToSolve, manifoldValidity, deltaTime, contactIter);
                    }
                }

                {
                    OPTICK_EVENT("Resolve friction constraints");

                    //resolve friction constraint
                    for (size_t frictionIter = 0;
                        frictionIter < constants::frictionSolverIterationCount; frictionIter++)
                    {
                        resolveFrictionConstraint(manifoldsToSolve, manifoldValidity);
                    }
                }

                {
                    OPTICK_EVENT("Converge manifolds");

                    //reset convergance identifiers for all colliders
                    for (auto& manifold : manifoldsToSolve)
                    {
                        manifold.colliderA->converganceIdentifiers.clear();
                        manifold.colliderB->converganceIdentifiers.clear();
                    }

                    //using the known lambdas of this time step, add it as a convergance identifier
                    for (auto& manifold : manifoldsToSolve)
                    {
                        for (auto& contact : manifold.contacts)
                        {
                            contact.refCollider->AddConverganceIdentifier(contact);
                        }
                    }
                }
            }

        }

        /**@brief given 2 physics_manifold_precursors precursorA and precursorB, create a manifold for each collider in precursorA
        * with every other collider in precursorB. The manifolds that involve rigidbodies are then pushed into the given manifold list
        * @param manifoldsToSolve [out] a std::vector of physics_manifold that will store the manifolds created
        * @param isRigidbodyInvolved A bool that indicates whether a rigidbody is involved in this manifold
        * @param isTriggerInvolved A bool that indicates whether a physicsComponent with a physicsComponent::isTrigger set to true is involved in this manifold
        */
        void constructManifoldsWithPrecursors(ecs::component_container<rigidbody>& rigidbodies, std::vector<byte>& hasRigidBodies, physics_manifold_precursor& precursorA, physics_manifold_precursor& precursorB,
            std::vector<physics_manifold>& manifoldsToSolve, bool isRigidbodyInvolved, bool isTriggerInvolved)
        {
            OPTICK_EVENT();
            auto& physicsComponentA = *precursorA.physicsComp;
            auto& physicsComponentB = *precursorB.physicsComp;

            for (auto& colliderA : physicsComponentA.colliders)
            {
                for (auto& colliderB : physicsComponentB.colliders)
                {
                    physics::physics_manifold m;
                    constructManifoldWithCollider(rigidbodies, hasRigidBodies, colliderA.get(), colliderB.get(), precursorA, precursorB, m);

                    if (!m.isColliding)
                    {
                        continue;
                    }

                    colliderA->PopulateContactPoints(colliderB.get(), m);

                    if (isTriggerInvolved)
                    {
                        //notify the event-bus
                        raiseEvent<trigger_event>(&m, m_timeStep);
                        //notify both the trigger and triggerer
                        //TODO:(Developer-The-Great): the triggerer and trigger should probably received this event
                        //TODO:(cont.) through the event bus, we should probably create a filterable system here to
                        //TODO:(cont.) uniquely identify involved objects and then redirect only required messages
                    }

                    if (isRigidbodyInvolved && !isTriggerInvolved)
                    {
                        raiseEvent<collision_event>(&m, m_timeStep);
                        manifoldsToSolve.emplace_back(std::move(m));
                    }
                }
            }
        }

        void constructManifoldWithCollider(
            ecs::component_container<rigidbody>& rigidbodies, std::vector<byte>& hasRigidBodies,
            PhysicsCollider* colliderA, PhysicsCollider* colliderB
            , physics_manifold_precursor& precursorA, physics_manifold_precursor& precursorB, physics_manifold& manifold)
        {
            OPTICK_EVENT();
            manifold.colliderA = colliderA;
            manifold.colliderB = colliderB;

            manifold.entityA = precursorA.entity;
            manifold.entityB = precursorB.entity;

            if (hasRigidBodies[precursorA.id])
                manifold.rigidbodyA = &rigidbodies[precursorA.id];
            else
                manifold.rigidbodyA = nullptr;

            if (hasRigidBodies[precursorB.id])
                manifold.rigidbodyB = &rigidbodies[precursorB.id];
            else
                manifold.rigidbodyB = nullptr;

            manifold.physicsCompA = precursorA.physicsComp;
            manifold.physicsCompB = precursorB.physicsComp;

            manifold.transformA = precursorA.worldTransform;
            manifold.transformB = precursorB.worldTransform;

            // log::debug("colliderA->CheckCollision(colliderB, manifold)");
            colliderA->CheckCollision(colliderB, manifold);
        }

        /** @brief gets all the entities with a rigidbody component and calls the integrate function on them
        */
        void integrateRigidbodies(std::vector<byte>& hasRigidBodies, ecs::component_container<rigidbody>& rigidbodies, float deltaTime)
        {
            OPTICK_EVENT();
            m_scheduler->queueJobs(manifoldPrecursorQuery.size(), [&]() {
                if (!hasRigidBodies[async::this_job::get_id()])
                    return;

                auto& rb = rigidbodies[async::this_job::get_id()];

                ////-------------------- update velocity ------------------//
                math::vec3 acc = rb.forceAccumulator * rb.inverseMass;
                rb.velocity += (acc + constants::gravity) * deltaTime;

                ////-------------------- update angular velocity ------------------//
                math::vec3 angularAcc = rb.torqueAccumulator * rb.globalInverseInertiaTensor;
                rb.angularVelocity += (angularAcc)*deltaTime;

                rb.resetAccumulators();
                }).wait();
        }

        void integrateRigidbodyQueryPositionAndRotation(
            std::vector<byte>& hasRigidBodies,
            ecs::component_container<position>& positions,
            ecs::component_container<rotation>& rotations,
            ecs::component_container<rigidbody>& rigidbodies,
            float deltaTime)
        {
            OPTICK_EVENT();
            m_scheduler->queueJobs(manifoldPrecursorQuery.size(), [&]() {
                id_type index = async::this_job::get_id();
                if (!hasRigidBodies[index])
                    return;

                auto& rb = rigidbodies[index];
                auto& pos = positions[index];
                auto& rot = rotations[index];

                ////-------------------- update position ------------------//
                pos += rb.velocity * deltaTime;

                ////-------------------- update rotation ------------------//
                float angle = math::clamp(math::length(rb.angularVelocity), 0.0f, 32.0f);
                float dtAngle = angle * deltaTime;

                if (!math::epsilonEqual(dtAngle, 0.0f, math::epsilon<float>()))
                {
                    math::vec3 axis = math::normalize(rb.angularVelocity);

                    math::quat glmQuat = math::angleAxis(dtAngle, axis);
                    rot = glmQuat * rot;
                    rot = math::normalize(rot);
                }

                //for now assume that there is no offset from bodyP
                rb.globalCentreOfMass = pos;

                rb.UpdateInertiaTensor(rot);
                }).wait();
        }

        void initializeManifolds(std::vector<physics_manifold>& manifoldsToSolve, std::vector<byte>& manifoldValidity)
        {
            OPTICK_EVENT();
            for (int i = 0; i < manifoldsToSolve.size(); i++)
            {
                if (manifoldValidity.at(i))
                {
                    auto& manifold = manifoldsToSolve.at(i);

                    for (auto& contact : manifold.contacts)
                    {
                        contact.preCalculateEffectiveMass();
                        contact.ApplyWarmStarting();
                    }
                }

            }
        }

        void resolveContactConstraint(std::vector<physics_manifold>& manifoldsToSolve, std::vector<byte>& manifoldValidity, float dt, int contactIter)
        {
            OPTICK_EVENT();

            for (int manifoldIter = 0;
                manifoldIter < manifoldsToSolve.size(); manifoldIter++)
            {
                if (manifoldValidity.at(manifoldIter))
                {
                    auto& manifold = manifoldsToSolve.at(manifoldIter);

                    for (auto& contact : manifold.contacts)
                    {
                        contact.resolveContactConstraint(dt, contactIter);
                    }
                }
            }
        }

        void resolveFrictionConstraint(std::vector<physics_manifold>& manifoldsToSolve, std::vector<byte>& manifoldValidity)
        {
            OPTICK_EVENT();

            for (int manifoldIter = 0;
                manifoldIter < manifoldsToSolve.size(); manifoldIter++)
            {
                if (manifoldValidity.at(manifoldIter))
                {
                    auto& manifold = manifoldsToSolve.at(manifoldIter);

                    for (auto& contact : manifold.contacts)
                    {
                        contact.resolveFrictionConstraint();
                    }
                }
            }
        }

    };
}

