#include <core/serialization/view_type.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename type>
    inline json json_view<type>::serialize(type object)
    {
        json j;
        for_each(object,
            [&j](auto& name, auto& value)
            {
                j[name] = value;
            });
        return j;
    }

    template<typename type>
    inline component_prototype<type> json_view<type>::deserialize(json j)
    {
        component_prototype<type> prot;
        for_each(prot,
            [&j](auto& name, auto& value)
            {
                value = j[name];
            });

        return prot;
    }
}

