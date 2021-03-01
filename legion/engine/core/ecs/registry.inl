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
        return *reinterpret_cast<component_type*>(getFamily(make_hash<component_type>())->create_component(target));
    }

    template<typename component_type>
    inline component_type& Registry::createComponent(entity target, const serialization::component_prototype<component_type>& prototype)
    {
        return *reinterpret_cast<component_type*>(getFamily(make_hash<component_type>())->create_component(target, prototype));
    }

    template<typename component_type>
    inline void Registry::destroyComponent(entity target)
    {
        getFamily(make_hash<component_type>())->destroy_component(target);
    }

    template<typename component_type>
    inline component_type& Registry::getComponent(entity target)
    {
        return *reinterpret_cast<component_type*>(getFamily(typeId)->get_component(target));
    }

}
