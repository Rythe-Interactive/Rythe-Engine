#include <core/serialization/view_type.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename comp_prototype>
    inline json json_view<comp_prototype>::serialize(const comp_prototype object)
    {
        json j;
        for_each(object,
            [&j](auto& name, auto& value)
            {
                j[name] = value;
            });
        return j;
    }

    template<typename comp_prototype>
    inline comp_prototype json_view<comp_prototype>::deserialize(const json j)
    {
        comp_prototype prot;
        for_each(prot,
            [&j](auto& name, auto& value)
            {
                value = j[name];
            });
        return prot;
    }
}

