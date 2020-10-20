#pragma once
#include <tuple>
#include <core/platform/platform.hpp>
#include <core/types/primitives.hpp>

/**
 * @file archetype.hpp
 */

namespace legion::core::ecs
{
    template<typename component_type>
    class component_handle;

    class EcsRegistry;

    /**@class archetype_base
     * @brief Common base class of all archetypes. Allows SFINAE checks for whether a templated type is an archetype.
     */
    struct archetype_base {};

    /**@class archetype
     * @brief API object for grouping multiple component types together.
     * @ref legion::core::transform
     */
    template<typename component_type, typename... component_types>
    struct archetype : public archetype_base
    {
        friend class EcsRegistry;
    public:
        using handleGroup = std::tuple<component_handle<component_type>, component_handle<component_types>...>;

        handleGroup handles;

        archetype(const handleGroup& handles) : handles(handles) {}

        /**@brief Get the handle to one of the components in the archetype.
         */
        template<typename T>
        component_handle<T> get()
        {
            return std::get<component_handle<T>>(handles);
        }

    private:
        static std::tuple<component_handle<component_type>, component_handle<component_types>...> create(EcsRegistry* registry, id_type entityId);
        static std::tuple<component_handle<component_type>, component_handle<component_types>...> create(EcsRegistry* registry, id_type entityId, component_type&& defaultValue, component_types&&... defaultValues);

        static std::tuple<component_handle<component_type>, component_handle<component_types>...> get(EcsRegistry* registry, id_type entityId);
        static void destroy(EcsRegistry* registry, id_type entityId);
        static bool has(EcsRegistry* registry, id_type entityId);
    };
}
