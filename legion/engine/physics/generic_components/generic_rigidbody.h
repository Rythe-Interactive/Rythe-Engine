#pragma once
#include <physics/dataTemp/rigibody_data.hpp>

namespace legion::physics
{
    constexpr size_type invalid_rigidbody = 0;

    class rigibodyDataHandle
    {
    public:

        rigibodyDataHandle(core::pointer<rigidbodyData> rigidbodyDataPtr) : m_rigidbodyDataPtr(rigidbodyDataPtr)
        {

        }

        core::pointer<rigidbodyData> Get() const { return m_rigidbodyDataPtr; }

        ~rigibodyDataHandle()
        {
            m_rigidbodyDataPtr->sendModifyEvent();
        }

    private:

        core::pointer<rigidbodyData> m_rigidbodyDataPtr{ nullptr };
    };

    class genericRigidbody
    {
    public:

        rigibodyDataHandle createDataHandle() 
        {
            core::pointer<rigidbodyData> rigidbodyDataPtr;
            rigidbodyDataPtr.ptr = &m_rigidbodyData;

            return rigibodyDataHandle(rigidbodyDataPtr);
        }

        const& rigidbodyData GetRigidbodyReadOnly() const { return m_rigidbodyData; }

    private:

        rigidbodyData m_rigidbodyData;
        size_type rigidbodyIndex = invalid_rigidbody;
    };
}
