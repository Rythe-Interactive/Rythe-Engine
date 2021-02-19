#pragma once

namespace legion::core::ecs
{
    struct component_base
    {

    };

    template<typename component_type>
    struct component : public component_base
    {

    };
}
