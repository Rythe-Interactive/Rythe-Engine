#pragma once

#include <core/core.hpp>
#include <rsl/tuple>

namespace sandbox
{
    RYTHE_DECLARE_MODULE(sandbox);

    struct hierarchy
    {
        rythe::core::entity parent;
    };

    struct physics_config
    {
        rsl::time_span updateInterval = rsl::time_span::from_milliseconds(25);
    };

    struct position
    {
        position() = default;
        position(const rsl::math::float3& src)
            : value(src)
        {}

        operator rsl::math::float3() { return value; }

        rsl::math::float3 value;
    };

    struct rotation
    {
        rotation() = default;
        rotation(const rsl::math::quat& src)
            : value(src)
        {}

        operator rsl::math::quat() { return value; }

        rsl::math::quat value;
    };

    struct scale
    {
        scale() = default;
        scale(const rsl::math::float3& src)
            : value(src)
        {}

        operator rsl::math::float3() { return value; }

        rsl::math::float3 value;
    };

    struct transform : rythe::architype<position, rotation, scale>
    {
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
