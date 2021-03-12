#pragma once
#include <unordered_map>

#include <core/platform/platform.hpp>
#include <core/containers/hashed_sparse_set.hpp>
#include <core/types/primitives.hpp>

#include <core/ecs/handles/entity.hpp>
#include <core/ecs/handles/filter.hpp>
#include <core/ecs/filters/filter_info.hpp>

/**
 * @file filterregistry.hpp
 */

namespace legion::core::ecs
{
    /**@class FilterRegistry
     * @brief 
     */
    class FilterRegistry
    {
    public:
        template<typename... component_types>
        friend struct filter;
        template<typename... component_types>
        friend struct filter_info;

        /**@brief 
         * @param componentId
         * @param target
         */
        static void markComponentAdd(id_type componentId, entity target);

        /**@brief 
         * @param componentId
         * @param target
         */
        static void markComponentErase(id_type componentId, entity target);

        /**@brief 
         * @tparam component_type
         * @param target 
         */
        template<typename component_type>
        static void markComponentAdd(entity target);

        /**@brief
         * @tparam component_type
         * @param target
         */
        template<typename component_type>
        static void markComponentErase(entity target);

        /**@brief 
         * @param target
         */
        static void markEntityDestruction(entity target);

        /**@brief 
         * @param target
         */
        static void markEntityFullCreation(entity target);

        /**@brief 
         * @param filterId
         * @return 
         */
        static entity_set& getList(id_type filterId);

    private:
        L_NODISCARD static std::unordered_map<id_type, entity_set>& entityLists() noexcept;

        L_NODISCARD static std::vector<std::unique_ptr<filter_info_base>>& filters() noexcept;

        template<typename component_type>
        constexpr static id_type generateId() noexcept;

        template<typename component_type0, typename component_type1, typename... component_types>
        constexpr static id_type generateId() noexcept;

        template<typename... component_types>
        static const id_type generateFilterImpl();

        template<typename... component_types>
        static const id_type generateFilter();
    };
}

#include <core/ecs/filters/filter_info.inl>
#include <core/ecs/handles/filter.inl>
