#pragma once

#ifndef ARGS_IMPORT
    #define ARGS_IMPORT
    #include <core/core.hpp>
    #include <core/platform/args_library.hpp>
#else
    #include <application/application.hpp>
#endif // !ARGS_IMPORT

namespace args::physics
{
    struct rigidbody
    {
        math::mat3 inverseInertiaTensor = math::mat3(1.0f);

        math::vec3 velocity;
        math::vec3 acc;

        math::vec3 angularAcc;
        math::vec3 angularVelocity;

        float inverseMass = 1.0f;

        float restitution;
        float friction;

        bool isAsleep;

        void addForce(math::vec3 force)
        {

        }

        void addForceAt(math::vec3 worldPosition, math::vec3 force)
        {

        }

        void integrate(float deltaTick)
        {

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

        

   






    };
}
