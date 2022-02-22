#pragma once
#include <physics/dataTemp/rigibody_data.hpp>

namespace legion::physics
{
    constexpr size_type invalid_rigidbody = 0;

    class genericRigidbody
    {
    public:

        rigidbodyData rigidbodyData;

    private:

        size_type rigidbodyIndex = invalid_rigidbody;
    };
}
