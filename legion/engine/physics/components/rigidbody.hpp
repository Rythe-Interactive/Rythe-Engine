#pragma once


#include <core/core.hpp>
#include <physics/physicsconstants.h>
#include <physics/components/physics_component.hpp>

namespace legion::physics
{
    struct rigidbody
    {
        //linear motion component
        float inverseMass = 1.0f;
        math::vec3 velocity;
        math::vec3 acc;
        float linearDrag;

        //angular motion component
        math::mat3 inverseInertiaTensor = math::mat3(12.0f);

        math::vec3 angularAcc;
        math::vec3 angularVelocity;
        float angularDrag = 0.01f;

        //force application component
        math::vec3 globalCentreOfMass;
        math::vec3 forceAccumulator;
        math::vec3 torqueAccumulator;

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
            restitution = math::clamp(newRestitution, 0.0f, 1.0f);
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
