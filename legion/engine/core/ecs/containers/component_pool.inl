#include <core/ecs/registry.hpp>
#include <core/ecs/containers/component_pool.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type>
    void* component_pool<component_type>::this_ptr = Registry::tryEmplaceFamily<component_type>();
    
    template<typename component_type>
    sparse_map<id_type, component_type> component_pool<component_type>::m_components = sparse_map<id_type, component_type>(125);

    template<typename component_type>
    inline L_ALWAYS_INLINE void component_pool<component_type>::clear()
    {
        m_components.clear();
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void* component_pool<component_type>::create_component(id_type target)
    {
        return &(m_components.try_emplace(target).first.value());
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void* component_pool<component_type>::create_component(id_type target, const serialization::component_prototype_base& prototype)
    {
        return &(m_components.try_emplace(target,
            from_reflector(static_cast<const serialization::component_prototype<component_type>&>(prototype))
        ).first.value());
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void* component_pool<component_type>::create_component(id_type target, serialization::component_prototype_base&& prototype)
    {
        return &(m_components.try_emplace(target,
            from_reflector(static_cast<serialization::component_prototype<component_type>&&>(prototype))
        ).first.value());
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE bool component_pool<component_type>::contains(id_type target) const
    {
        return m_components.contains(target);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void component_pool<component_type>::destroy_component(id_type target)
    {
        m_components.erase(target);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE std::unique_ptr<serialization::component_prototype_base> component_pool<component_type>::create_prototype(id_type target) const
    {
        return std::unique_ptr<serialization::component_prototype_base>(
            new serialization::component_prototype<component_type>(m_components.at(target))
            );
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE serialization::component_prototype<component_type> component_pool<component_type>::create_prototype_direct(id_type target)
    {
        return serialization::component_prototype<component_type>(m_components.at(target));
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void* component_pool<component_type>::get_component(id_type target) const
    {
        return &(m_components.at(target));
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& component_pool<component_type>::create_component_direct(id_type target)
    {
        return m_components.try_emplace(target).first.value();
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& component_pool<component_type>::create_component_direct(id_type target, component_type&& value)
    {
        return m_components.try_emplace(target, std::forward<component_type>(value)).first.value();

    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& component_pool<component_type>::create_component_direct(id_type target, const component_type& value)
    {
        return m_components.try_emplace(target, value).first.value();
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& component_pool<component_type>::create_component_direct(id_type target, const serialization::component_prototype_base& prototype)
    {
        return m_components.try_emplace(target,
            from_reflector(static_cast<const serialization::component_prototype<component_type>&>(prototype))
        ).first.value();
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& component_pool<component_type>::create_component_direct(id_type target, serialization::component_prototype_base&& prototype)
    {
        return m_components.try_emplace(target,
            from_reflector(static_cast<serialization::component_prototype<component_type>&&>(prototype))
        ).first.value();
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE bool component_pool<component_type>::contains_direct(id_type target)
    {
        return m_components.contains(target);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& component_pool<component_type>::get_component_direct(id_type target)
    {
        return m_components.at(target);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void component_pool<component_type>::destroy_component_direct(id_type target)
    {
        m_components.erase(target);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void component_pool<component_type>::fill_container(component_container<component_type>& container, entity_set& entities)
    {
        container.reserve(entities.size());

        for (auto ent : entities)
            container.emplace_back(m_components.at(ent));
    }

}
