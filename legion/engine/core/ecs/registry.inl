#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type, typename ...Args>
    inline component_pool<component_type>* Registry::tryEmplaceFamily(Args && ...args)
    {
        return m_componentFamilies.try_emplace(make_hash<component_type>(), std::forward<Args>(args)...).first->get();
    }

    template<typename component_type>
    inline component_pool<component_type>* ecs::Registry::getFamily()
    {
        return tryEmplaceFamily<component_type>();
    }

    template<typename component_type>
    inline component_type& Registry::createComponent(entity target)
    {
        return component_pool<component_type>::create_component_direct(target);
    }

    template<typename component_type>
    inline component_type& Registry::createComponent(entity target, const serialization::component_prototype<component_type>& prototype)
    {
        return component_pool<component_type>::create_component_direct(target, prototype);
    }

    template<typename component_type>
    inline void Registry::destroyComponent(entity target)
    {
        component_pool<component_type>::destroy_component_direct(target);
    }

    template<typename component_type>
    inline component_type& Registry::getComponent(entity target)
    {
        return component_pool<component_type>::get_component_direct(target);
    }

}
