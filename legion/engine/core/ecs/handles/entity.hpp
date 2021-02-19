#pragma once
#include <core/types/types.hpp>

namespace legion::core::ecs
{
    struct entity
    {
    public:
        id_type id;

        operator const id_type& () const;
        operator id_type& ();
    };
}
