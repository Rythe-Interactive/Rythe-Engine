#include <core/serialization/view_type.hpp>
#pragma once

namespace legion::core::serialization
{
    template<typename type>
    inline json json_view<type>::serialize(type object)
    {
        return json();
    }

    template<typename type>
    inline prototype_base json_view<type>::deserialize(json j)
    {
        return prototype<type>();
    }
}
