#include <core/ecs/handles/filter.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename... ComponentTypes>
    const id_type filter<ComponentTypes...>::id = FilterRegistry::generateFilter<ComponentTypes...>();

    template<typename... ComponentTypes>
    std::tuple<component_container<ComponentTypes>...> filter<ComponentTypes...>::m_containers;

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::iterator filter<ComponentTypes...>::begin() noexcept
    {
        return FilterRegistry::getList(id).begin();
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::iterator filter<ComponentTypes...>::end() noexcept
    {
        return FilterRegistry::getList(id).end();
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::reverse_iterator filter<ComponentTypes...>::rbegin() noexcept
    {
        return FilterRegistry::getList(id).rbegin();
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::reverse_iterator filter<ComponentTypes...>::rend() noexcept
    {
        return FilterRegistry::getList(id).rend();
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::reverse_itr_range filter<ComponentTypes...>::reverse_range() noexcept
    {
        return FilterRegistry::getList(id).reverse_range();
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE size_type filter<ComponentTypes...>::size() noexcept
    {
        return FilterRegistry::getList(id).size();
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE bool filter<ComponentTypes...>::empty() noexcept
    {
        return FilterRegistry::getList(id).empty();
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE size_type filter<ComponentTypes...>::count(entity val)
    {
        return FilterRegistry::getList(id).count(val);
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE size_type filter<ComponentTypes...>::contains(entity val)
    {
        return FilterRegistry::getList(id).contains(val);
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::iterator filter<ComponentTypes...>::find(entity val)
    {
        return FilterRegistry::getList(id).find(val);
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE entity& filter<ComponentTypes...>::at(size_type index)
    {
        return FilterRegistry::getList(id).at(index);
    }

    template<typename... ComponentTypes>
    L_NODISCARD inline L_ALWAYS_INLINE entity& filter<ComponentTypes...>::operator[](size_type index)
    {
        return FilterRegistry::getList(id).at(index);
    }

    template<typename... ComponentTypes>
    template<typename ComponentType>
    L_NODISCARD inline L_ALWAYS_INLINE component_container<ComponentType>& filter<ComponentTypes...>::get()
    {
        auto& container = std::get<component_container<ComponentType>>(m_containers);
        container.clear();
        reinterpret_cast<component_pool<ComponentType>*>(Registry::getFamily(typeHash<ComponentType>()))->fill_container(container, FilterRegistry::getList(id));
        return container;
    }
}
