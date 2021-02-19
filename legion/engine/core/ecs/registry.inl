#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type, typename ...Args>
    inline L_NODISCARD component_pool<component_type>* Registry::tryEmplaceFamily(Args && ...args)
    {
        return nullptr;
    }

    template<typename component_type>
    inline L_NODISCARD component_pool<component_type>* ecs::Registry::getFamily()
    {
        return tryEmplaceFamily<component_type>();
    }

}
