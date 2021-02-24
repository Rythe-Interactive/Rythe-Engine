#include <core/ecs/containers/component_pool.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type>
    inline void* component_pool<component_type>::create_component(id_type target)
    {
        return &*(m_components.emplace(target).first);
    }

    template<typename component_type>
    inline void* component_pool<component_type>::create_component(id_type target, const serialization::component_prototype_base& prototype)
    {
        return &*(m_components.emplace(target,
            from_reflector(static_cast<const serialization::component_prototype<component_type>&>(prototype))
        ).first);
    }

    template<typename component_type>
    inline void component_pool<component_type>::destroy_component(id_type target)
    {
        m_components.erase(target);
    }

    template<typename component_type>
    inline void* component_pool<component_type>::get_component(id_type target)
    {
        return &(m_components.at(target));
    }

}
