#include <physics/dataTemp/rigibody_data.hpp>
#include <physics/events/events.hpp>

namespace legion::physics
{
    //multithreaded physics engines (such as physx) dont allow writing to the physics engine when
    //a physics step is being executed. Because of this, we instead store the events and whichever
    //physics engine being used will process these events manually.

    inline void rigidbodyData::setMass(float mass)
    {
        m_mass = mass;
        m_modifyRigidbodyEvents.push_back(std::make_unique<rb_modify_mass>(mass));
    }

    inline void rigidbodyData::setInertiaTensor(const math::mat3& inertiaTensor)
    {
        m_inertiaTensor = inertiaTensor;
        m_modifyRigidbodyEvents.push_back(std::make_unique<rb_modify_inertia_tensor>(inertiaTensor));
    }

    inline void rigidbodyData::setVelocity(const math::vec3& velocity)
    {
        m_velocity = velocity;
        m_modifyRigidbodyEvents.push_back(std::make_unique<rb_modify_velocity>(velocity));
    }

    inline void rigidbodyData::setLinearDrag(float linearDrag)
    {
        m_linearDrag = linearDrag;
        m_modifyRigidbodyEvents.push_back(std::make_unique<rb_modify_linear_drag>(linearDrag));
    }

    inline void rigidbodyData::setAngularDrag(float angularDrag)
    {
        m_angularDrag = angularDrag;
        m_modifyRigidbodyEvents.push_back(std::make_unique<rb_modify_angular_drag>(angularDrag));
    }
}
