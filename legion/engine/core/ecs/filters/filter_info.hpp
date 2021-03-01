#pragma once
#include <array>
#include <unordered_set>

#include <core/common/hash.hpp>

namespace legion::core::ecs
{
    struct filter_info_base
    {
        id_type id;

        template<typename component>
        bool contains() { return contains(make_hash<T>()); }

        virtual bool contains(id_type id) LEGION_PURE;
        virtual bool contains(const std::unordered_set<id_type>& comp) LEGION_PURE;
    };

    template<typename... component_types>
    struct filter_info : public filter_info_base
    {
        static constexpr std::array<id_type, sizeof...(component_types)> composition = { make_hash<component_types>()... };

        virtual bool contains(id_type id)
        {
            return contains_impl(id);
        }

        virtual bool contains(const std::unordered_set<id_type>& comp)
        {
            for (auto& typeId : composition)
                if (!comp.count(typeId))
                    return false;
            return true;
        }

    private:
        constexpr bool contains_impl(id_type id)
        {
            for (int i = 0; i < composition.size(); i++)
                if (composition[i] == id)
                    return true;
            return false;
        }
    };
}
