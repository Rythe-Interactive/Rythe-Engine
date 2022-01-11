#include <core/ecs/filters/filter_info.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename... component_types>
    const id_type filter_info<component_types...>::filter_id = filter_info<component_types...>::generateId<component_types...>();

    template<typename... component_types>
    const std::array<id_type, sizeof...(component_types)> filter_info<component_types...>::composition = { make_hash<component_types>().id()... };

    template<typename... component_types>
    inline L_ALWAYS_INLINE id_type filter_info<component_types...>::id()
    {
        return filter_id;
    }

    template<typename... component_types>
    inline L_ALWAYS_INLINE bool filter_info<component_types...>::contains(id_type id) noexcept
    {
        return contains_direct(id);
    }

    template<typename... component_types>
    inline L_ALWAYS_INLINE bool filter_info<component_types...>::contains(const std::unordered_set<id_type>& comp)
    {
        if (!comp.size())
            return false;

        if (!composition.size())
            return true;

        for (auto& typeId : composition)
            if (!comp.count(typeId))
                return false;
        return true;
    }

    template<typename... component_types>
    constexpr bool filter_info<component_types...>::contains_direct(id_type id) noexcept
    {
        for (int i = 0; i < composition.size(); i++)
            if (composition[i] == id)
                return true;
        return false;
    }
}
