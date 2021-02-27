#include <core/ecs/handles/component.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type>
    inline component<component_type>::operator component_type& ()
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline component<component_type>::operator const component_type& () const
    {
        return Registry::getComponent<component_type>(owner);
    }

}
