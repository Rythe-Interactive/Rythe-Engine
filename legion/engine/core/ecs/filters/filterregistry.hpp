#pragma once
#include <unordered_map>

#include <core/platform/platform.hpp>
#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/primitives.hpp>
#include <core/engine/enginesubsystem.hpp>

#include <core/ecs/handles/entity.hpp>
#include <core/ecs/filters/filter_info.hpp>

/**
 * @file filterregistry.hpp
 */

namespace legion::core::ecs
{
    template<typename... component_types>
    struct filter;

    /**@class FilterRegistry
     * @brief Manager and owner of all entity filter related data.
     */
    class FilterRegistry : public EngineSubSystem<FilterRegistry>
    {
        ALLOW_PRIVATE_ONINIT;
        ALLOW_PRIVATE_ONSHUTDOWN;
    public:
        template<typename... component_types>
        friend struct filter_info;

        template<typename... component_types>
        friend struct filter;

        /**@brief Message that a certain component was added to an entity. This will update all filters that might be interested in this entity.
         * @param componentId Id of the component type.
         * @param target Entity that was changed.
         */
        static void markComponentAdd(id_type componentId, entity target);

        /**@brief Message that a certain component was removed from an entity. This will update all filters that might be interested in this entity.
         * @param componentId Id of the component type.
         * @param target Entity that was changed.
         */
        static void markComponentErase(id_type componentId, entity target);

        /**@brief Message that a certain component was added to an entity. This will update all filters that might be interested in this entity.
         * @tparam component_type Type of component that was added.
         * @param target Entity that was changed.
         */
        template<typename component_type>
        static void markComponentAdd(entity target);

        /**@brief Message that a certain component was removed from an entity. This will update all filters that might be interested in this entity.
         * @tparam component_type Type of component that was removed.
         * @param target Entity that was changed.
         */
        template<typename component_type>
        static void markComponentErase(entity target);

        /**@brief Message that a certain entity was destroyed. This will erase the entity from all interested filters.
         * @param target Entity that was destroyed.
         */
        static void markEntityDestruction(entity target);

        /**@brief Message that a certain entity was created with components already applied. This will insert the entity to all interested filters.
         * @param target Entity that was created.
         */
        static void markEntityFullCreation(entity target);

        /**@brief Fetch the list of entities that apply to a certain filter.
         * @param filterId Id of the filter.
         */
        static entity_set& getList(id_type filterId);

    private:
        static void onInit();
        static void onShutdown();

        std::unordered_map<id_type, entity_set> m_entityLists;

        std::vector<std::unique_ptr<filter_info_base>> m_filters;

        template<typename component_type>
        constexpr static id_type generateId() noexcept;

        template<typename component_type0, typename component_type1, typename... component_types>
        constexpr static id_type generateId() noexcept;

        template<typename... component_types>
        static id_type generateFilterImpl();

        template<typename... component_types>
        static id_type generateFilter();
    };
}

#include <core/ecs/filters/filter_info.inl>
