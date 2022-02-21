#pragma once
#include <core/core.hpp>

namespace legion::physics
{
    class rigidbodyData
    {
    public:

        friend class rigibodyDataHandle;

        inline void setMass(float mass)
        {
            m_mass = mass;
        }

        inline void setInertiaTensor(const math::mat3& inertiaTensor)
        {
            m_inertiaTensor = inertiaTensor;
        }

        inline void setVelocity(const math::vec3& velocity)
        {
            m_velocity = velocity;
        }

        inline void setDrag(float linearDrag)
        {
            m_linearDrag = linearDrag;
        }

        inline void setAngularDrag(float angularDrag)
        {
            m_angularDrag = angularDrag;
        }

        inline float getMass() { return m_mass; }

        inline math::mat3 getInertiaTensor() { return m_inertiaTensor; }

        inline math::vec3 getVelocity() { return m_velocity; }

        inline float getDrag() { return m_linearDrag; }

        inline float getAngularDrag() { return m_angularDrag; }

    private:

        inline void sendModifyEvent();

        math::vec3 m_velocity = math::vec3(0.0);
        math::vec3 m_angularVelocity = math::vec3(0.0);

        math::mat3 m_inertiaTensor = math::mat3(6.0f);

        float m_mass = 1.0f;
        float m_linearDrag;
        float m_angularDrag = 0.01f;
    };
}
