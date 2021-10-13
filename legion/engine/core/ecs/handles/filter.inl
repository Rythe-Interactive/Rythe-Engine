#include <core/ecs/handles/filter.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename... component_types>
    const id_type filter<component_types...>::id = FilterRegistry::generateFilter<component_types...>();

    template<typename... component_types>
    std::tuple<component_container<component_types>...> filter<component_types...>::m_containers;

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::iterator filter<component_types...>::begin() noexcept
    {
        return FilterRegistry::getList(id).begin();
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::iterator filter<component_types...>::end() noexcept
    {
        return FilterRegistry::getList(id).end();
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::reverse_iterator filter<component_types...>::rbegin() noexcept
    {
        return FilterRegistry::getList(id).rbegin();
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::reverse_iterator filter<component_types...>::rend() noexcept
    {
        return FilterRegistry::getList(id).rend();
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::reverse_itr_range filter<component_types...>::reverse_range() noexcept
    {
        return FilterRegistry::getList(id).reverse_range();
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE size_type filter<component_types...>::size() noexcept
    {
        return FilterRegistry::getList(id).size();
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE bool filter<component_types...>::empty() noexcept
    {
        return FilterRegistry::getList(id).empty();
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE size_type filter<component_types...>::count(entity val)
    {
        return FilterRegistry::getList(id).count(val);
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE size_type filter<component_types...>::contains(entity val)
    {
        return FilterRegistry::getList(id).contains(val);
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE entity_set::iterator filter<component_types...>::find(entity val)
    {
        return FilterRegistry::getList(id).find(val);
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE entity& filter<component_types...>::at(size_type index)
    {
        return FilterRegistry::getList(id).at(index);
    }

    template<typename... component_types>
    L_NODISCARD inline L_ALWAYS_INLINE entity& filter<component_types...>::operator[](size_type index)
    {
        return FilterRegistry::getList(id).at(index);
    }

    template<typename... component_types>
    template<typename component_type>
    L_NODISCARD inline L_ALWAYS_INLINE component_container<component_type>& filter<component_types...>::get()
    {
        auto& container = std::get<component_container<component_type>>(m_containers);
        container.clear();
        ecs::component_pool<component_type>::fill_container(container, FilterRegistry::getList(id));
        return container;
    }
}
