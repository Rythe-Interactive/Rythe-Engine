#pragma once
#include <core/core.hpp>
#include <core/events/event.hpp>

namespace legion::physics
{
    class rigidbodyData
    {
    public:

        friend class rigibodyDataHandle;

        inline void setMass(float mass);

        inline void setInertiaTensor(const math::mat3& inertiaTensor);
        
        inline void setVelocity(const math::vec3& velocity);
        
        inline void setLinearDrag(float linearDrag);
       
        inline void setAngularDrag(float angularDrag);
        
        inline float getMass() { return m_mass; }

        inline math::mat3 getInertiaTensor() { return m_inertiaTensor; }

        inline math::vec3 getVelocity() { return m_velocity; }

        inline float getDrag() { return m_linearDrag; }

        inline float getAngularDrag() { return m_angularDrag; }

    private:

        math::vec3 m_velocity = math::vec3(0.0);
        math::vec3 m_angularVelocity = math::vec3(0.0);

        math::mat3 m_inertiaTensor = math::mat3(6.0f);

        float m_mass = 1.0f;
        float m_linearDrag;
        float m_angularDrag = 0.01f;

        std::vector<std::unique_ptr<core::events::event_base>> m_modifyRigidbodyEvents;
    };
}
