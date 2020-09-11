#pragma once

#ifndef ARGS_IMPORT
    #define ARGS_IMPORT
    #include <core/core.hpp>
    #include <core/platform/args_library.hpp>
#else
    #include <application/application.hpp>
#endif // !ARGS_IMPORT

#include <physics/physicsconstants.h>
#include <physics/physics_component.hpp>

namespace args::physics
{
    struct rigidbody
    {
        math::vec3 globalCentreOfMass;

        math::mat3 inverseInertiaTensor = math::mat3(12.0f);

        math::vec3 velocity;
        math::vec3 acc;

        math::vec3 angularAcc;
        math::vec3 angularVelocity;

        math::vec3 forceAccumulator;
        math::vec3 torqueAccumulator;

        float angularDrag = 0.01f;
        float linearDrag;

        float inverseMass = 1.0f;

        float restitution;
        float friction;

        bool isAsleep;


        
        /** @brief Pushes the rigidbody in the direction parallel to 'force' and equal to the
        * length of 'force'.
        * @note This does NOT cause torque
        */
        void addForce(math::vec3 force)
        {
            forceAccumulator += force;
        }

        /** @brief Adds a force in the direction of 'force' and
        */
        void addForceAt(math::vec3 worldForcePosition, math::vec3 force)
        {
            forceAccumulator += force;
            math::vec3 a = worldForcePosition - globalCentreOfMass;
            torqueAccumulator += math::cross(worldForcePosition - globalCentreOfMass, force);
        }

        void setMass(float mass)
        {
            inverseMass = 1.0f / mass;
        }

        void setRestitution(float newRestitution)
        {
            restitution = math::clamp(newRestitution,0.0f,1.0f);
        }

        void setFriction(float newFriction)
        {
            friction = math::clamp(newFriction, 0.0f, 1.0f);
        }

        void resetAccumulators()
        {
            forceAccumulator = math::vec3(0);
            torqueAccumulator = math::vec3(0);
        }


    };
}
