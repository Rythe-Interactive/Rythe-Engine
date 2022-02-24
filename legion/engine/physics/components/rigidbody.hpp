#pragma once

#include <core/core.hpp>
#include <physics/physicsconstants.hpp>


namespace legion::physics
{
    struct rigidbody
    {

        //linear motion component
        float inverseMass = 1.0f;
        math::float3 velocity = math::float3(0.0);
        math::float3 acc = math::float3(0.0);
        float linearDrag;

        //angular motion component
        math::mat3 localInverseInertiaTensor = math::mat3(6.0f);
        math::mat3 globalInverseInertiaTensor = localInverseInertiaTensor;

        math::float3 angularAcc = math::float3(0.0);
        math::float3 angularVelocity = math::float3(0.0);
        float angularDrag = 0.01f;

        //force application component
        math::float3 forceAccumulator = math::float3(0.0);
        math::float3 torqueAccumulator = math::float3(0.0);
        math::float3 globalCentreOfMass = math::float3(0.0);

        float restitution = 0.3f;
        float friction = 0.3f;

        bool isAsleep;

        template<typename Archive>
        void serialize(Archive& archive)
        {
            //We won't need to manually name all the components in the future
            archive(cereal::make_nvp("Name", std::string("Rigidbody")), cereal::make_nvp("Inverse Mass",inverseMass),
                        cereal::make_nvp("Velocity",velocity),cereal::make_nvp("Acceleration",acc), cereal::make_nvp("Linear Drag",linearDrag),
                        cereal::make_nvp("Inverse Intertia Tensor",localInverseInertiaTensor),cereal::make_nvp("Angular Acceleration",angularAcc),
                        cereal::make_nvp("Angular Velocity",angularVelocity),cereal::make_nvp("Angular Drag",angularDrag),cereal::make_nvp("Global Centre of Mass",globalCentreOfMass),
                        cereal::make_nvp("Force Accumulator",forceAccumulator),cereal::make_nvp("Torque Accumulator",torqueAccumulator),cereal::make_nvp("Restitution",restitution),
                        cereal::make_nvp("Friction",friction),cereal::make_nvp("Is Asleep?",isAsleep));
        }

        static float calculateRestitution(float restitutionA, float restitutionB)
        {
            return (restitutionA + restitutionB) / 2.0f;
        }

        static float calculateFriction(float frictionA, float frictionB)
        {
            return math::max(frictionA,frictionB);
        }

        /** @brief Pushes the rigidbody in the direction parallel to 'force' and equal to the
        * length of 'force'.
        * @note This does NOT cause torque
        */
        void addForce(math::float3 force)
        {
            forceAccumulator += force;
        }

        /** @brief Adds a force in the direction of 'force' and
        */
        void addForceAt(math::float3 worldForcePosition, math::float3 force)
        {
            forceAccumulator += force;
            math::float3 axis = worldForcePosition - globalCentreOfMass;
            torqueAccumulator += math::cross(axis, force);
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
            forceAccumulator = math::float3(0);
            torqueAccumulator = math::float3(0);
        }

        void UpdateInertiaTensor(math::quat orientation)
        {
            math::mat3 mat3Rot = math::toMat3(orientation);
            globalInverseInertiaTensor = math::inverse(mat3Rot) * localInverseInertiaTensor *  mat3Rot;
        }


    };
}
