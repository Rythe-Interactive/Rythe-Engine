#pragma once

#include <core/core.hpp>

namespace sandbox
{
    RYTHE_DECLARE_MODULE(sandbox);

    RYTHE_DECLARE_SYSTEM(sandbox, test_system);
    RYTHE_DECLARE_SYSTEM(sandbox, some_other_system);
    RYTHE_DECLARE_SYSTEM(sandbox, my_late_system);

    RYTHE_DECLARE_PUBLIC_PROCESS_CHAIN(test_system, update);
    RYTHE_DECLARE_PUBLIC_PROCESS_CHAIN(some_other_system, update);
    RYTHE_DECLARE_PUBLIC_PROCESS_CHAIN(my_late_system, update);

    struct hierarchy
    {
        rythe::core::entity parent;
    };

    using position = rsl::math::float3;

    struct physics_config
    {
        rsl::tm::span32 updateInterval;
    };

    struct transform
    {
        rsl::math::float3 position;
        rsl::math::quat rotation;
        rsl::math::float3 scale;
    };

    struct my_component
    {
        rsl::math::float3 rate;
    };

    struct my_other_component
    {
        rsl::math::float3 offset;
    };
}
