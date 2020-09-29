#pragma once
#include <tuple>
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>

namespace args::core::ecs
{
    template<typename component_type>
    class component_handle;

    class ARGS_API EcsRegistry;

    struct archetype_base {};

    template<typename component_type, typename... component_types>
    struct archetype : public archetype_base
    {
        friend class EcsRegistry;
    private:
        static std::tuple<component_handle<component_type>, component_handle<component_types>...> create(EcsRegistry* registry, id_type entityId);
        static std::tuple<component_handle<component_type>, component_handle<component_types>...> create(EcsRegistry* registry, id_type entityId, component_type&& defaultValue, component_types&&... defaultValues);

        static std::tuple<component_handle<component_type>, component_handle<component_types>...> get(EcsRegistry* registry, id_type entityId);
        static void destroy(EcsRegistry* registry, id_type entityId);
        static bool has(EcsRegistry* registry, id_type entityId);
    };
}
