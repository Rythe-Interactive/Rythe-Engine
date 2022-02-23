#pragma once
#include <physics/data/rigibody_data.hpp>

namespace legion::physics
{
    constexpr size_type invalid_rigidbody = 0;

    struct rigidbody
    {
        rigidbodyData rigidbodyData;
        size_type rigidbodyIndex = invalid_rigidbody;
    };
}
