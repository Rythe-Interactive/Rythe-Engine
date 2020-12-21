#pragma once

namespace legion::core::ecs
{
    template<typename component_type, typename... component_types, typename>
    L_NODISCARD bool entity_handle::has_components() const
    {
        OPTICK_EVENT();
        return m_registry->hasComponents<component_type, component_types...>(m_id);
    }

    template<typename archetype_type, typename>
    L_NODISCARD bool entity_handle::has_components() const
    {
        OPTICK_EVENT();
        return m_registry->hasComponents<archetype_type>(m_id);
    }

    template<typename component_type, typename... component_types, typename>
    L_NODISCARD auto ecs::entity_handle::get_component_handles() const
    {
        OPTICK_EVENT();
        return m_registry->getComponents<component_type, component_types...>(m_id);
    }

    template<typename archetype_type, typename>
    L_NODISCARD auto entity_handle::get_component_handles() const
    {
        OPTICK_EVENT();
        return m_registry->getComponents<archetype_type>(m_id);
    }

    template<typename component_type, typename... component_types, typename>
    auto entity_handle::add_components()
    {
        OPTICK_EVENT();
        return m_registry->createComponents<component_type, component_types...>(m_id);
    }

    template<typename archetype_type, typename>
    auto entity_handle::add_components()
    {
        OPTICK_EVENT();
        return m_registry->createComponents<archetype_type>(m_id);
    }

    template<typename component_type, typename... component_types, typename>
    auto entity_handle::add_components(component_type&& value, component_types&&... values)
    {
        OPTICK_EVENT();
        return m_registry->createComponents(m_id, std::forward<component_type>(value), std::forward<component_types>(values)...);
    }

    template<typename component_type, typename... component_types, typename>
    auto entity_handle::add_components(component_type& value, component_types&... values)
    {
        OPTICK_EVENT();
        return m_registry->createComponents(m_id, value, values...);
    }

    template<typename archetype_type, typename... component_types, typename>
    auto entity_handle::add_components(component_types&&... values)
    {
        OPTICK_EVENT();
        return m_registry->createComponents<archetype_type>(m_id, std::forward<component_types>(values)...);
    }

    template<typename component_type, typename... component_types, typename>
    void entity_handle::remove_components()
    {
        OPTICK_EVENT();
        return m_registry->destroyComponents<component_type, component_types...>(m_id);
    }

    template<typename archetype_type, typename>
    void entity_handle::remove_components()
    {
        OPTICK_EVENT();
        return m_registry->destroyComponents<archetype_type>(m_id);
    }
}
