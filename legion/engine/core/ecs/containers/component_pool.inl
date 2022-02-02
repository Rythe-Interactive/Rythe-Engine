#include <core/ecs/registry.hpp>
#include <core/ecs/containers/component_pool.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type>
    inline L_ALWAYS_INLINE void component_pool<component_type>::reserve(size_type size)
    {
        m_components.reserve(size);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void component_pool<component_type>::clear()
    {
        m_components.clear();
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void* component_pool<component_type>::create_component(entity target)
    {
        auto& ret = m_components.try_emplace(target).first.value();

        if constexpr (has_static_init_v<component_type, void(component_type&, entity)>)
        {
            component_type::init(ret, target);
        }
        else if constexpr (has_static_init_v<component_type, void(component_type&)>)
        {
            component_type::init(ret);
        }

        events::EventBus::raiseEvent<events::component_creation<component_type>>(target);

        return &ret;
    }

    template<typename component_type>
    template<typename ...Args>
    inline L_ALWAYS_INLINE component_type& component_pool<component_type>::create_component(entity target, Args&& ...args)
    {
        auto& ret = m_components.try_emplace(target, std::forward<Args>(args)...).first.value();

        if constexpr (has_static_init_v<component_type, void(component_type&, entity)>)
        {
            component_type::init(ret, target);
        }
        else if constexpr (has_static_init_v<component_type, void(component_type&)>)
        {
            component_type::init(ret);
        }

        events::EventBus::raiseEvent<events::component_creation<component_type>>(target);

        return ret;
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE bool component_pool<component_type>::contains(entity target) const
    {
        return m_components.contains(target);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void component_pool<component_type>::destroy_component(entity target)
    {
        events::EventBus::raiseEvent<events::component_destruction<component_type>>(target);

        if constexpr (has_static_destroy_v<component_type, void(component_type&, entity)>)
        {
            component_type::destroy(m_components.at(target->id), target);
        }
        else if constexpr (has_static_destroy_v<component_type, void(component_type&)>)
        {
            component_type::destroy(m_components.at(target->id));
        }

        m_components.erase(target->id);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void* component_pool<component_type>::get_component(entity target)
    {
        return &(m_components.at(target));
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE const void* component_pool<component_type>::get_component(entity target) const
    {
        return &(m_components.at(target));
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void component_pool<component_type>::fill_container(component_container<component_type>& container, entity_set& entities)
    {
        container.reserve(entities.size());

        for (auto ent : entities)
            container.emplace_back(m_components.at(ent));
    }

}
