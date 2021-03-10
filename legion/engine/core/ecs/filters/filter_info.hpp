#pragma once
#include <array>
#include <unordered_set>

#include <core/common/hash.hpp>

namespace legion::core::ecs
{
    struct filter_info_base
    {
        virtual id_type id() LEGION_PURE;

        template<typename component>
        bool contains() { return contains(make_hash<T>()); }

        virtual bool contains(id_type id) LEGION_PURE;
        virtual bool contains(const std::unordered_set<id_type>& comp) LEGION_PURE;
    };

    template<typename... component_types>
    struct filter_info : public filter_info_base
    {
    private:
        template<typename component_type>
        constexpr static id_type generateId() noexcept
        {
            return make_hash<component_type>();
        }

        template<typename component_type0, typename component_type1, typename... component_types>
        constexpr static id_type generateId() noexcept
        {
            return combine_hash(make_hash<component_type0>(), generateId<component_type1, component_types...>());
        }

    public:
        static constexpr id_type filter_id = generateId<component_types...>();
        static constexpr std::array<id_type, sizeof...(component_types)> composition = { make_hash<component_types>()... };

        virtual id_type id();

        virtual bool contains(id_type id) noexcept;

        virtual bool contains(const std::unordered_set<id_type>& comp);

        constexpr static bool contains_direct(id_type id) noexcept;
    };
}
