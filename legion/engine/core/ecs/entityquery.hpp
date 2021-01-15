#pragma once
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/hashed_sparse_set.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/ecs/archetype.hpp>
#include <core/ecs/component_container.hpp>

/**
 * @file entityquery.hpp
 */

namespace legion::core::ecs
{
    class QueryRegistry;
    class EcsRegistry;
    using entity_container = std::vector<entity_handle>;

    /**@class EntityQuery
     * @brief Handle to an entity query. Allows you to acquire a list all of entities with a certain component combination.
     */
    class EntityQuery
    {
    private:
        QueryRegistry* m_registry;
        EcsRegistry* m_ecsRegistry;
        id_type m_id;
        const entity_container* m_localcopy;

    public:
        EntityQuery(id_type id, QueryRegistry* registry, EcsRegistry* ecsRegistry);
        EntityQuery() = default;
        ~EntityQuery();

        EntityQuery(EntityQuery&& other);
        EntityQuery(const EntityQuery& other);

        EntityQuery operator=(EntityQuery&& other);
        EntityQuery operator=(const EntityQuery& other);

        component_container_base& get(id_type componentTypeId);

        template<typename component_type>
        component_container<component_type>& get()
        {
            return get(typeHash<component_type>()).template cast<component_type>();
        }

        void submit(id_type componentTypeId);

        template<typename component_type>
        void submit()
        {
            submit(typeHash<component_type>());
        }

        /**@brief Update the local copy of the entity list according to the query.
         */
        void queryEntities();

        /**@brief Get begin iterator for entity handles to the queried entities.
         */
        entity_container::const_iterator begin() const;

        /**@brief Get end iterator for entity handles to the queried entities.
         */
        entity_container::const_iterator end() const;

        /**@brief Get query id.
         */
        L_NODISCARD id_type id() { return m_id; }

        template <typename archetype>
        void addArchetype()
        {
            addArchetypeImpl<archetype>(std::make_index_sequence<std::tuple_size<archetype>::value>{});
        }
    private:
        template <typename archetype,size_t ... I>
        void addArchetypeImpl(std::index_sequence<I...>)
        {
            (addComponentType(typeHash<std::tuple_element<I,archetype>>),...);
        }
    public:

        /**@brief Add component type to query for.
         * @tparam component_type
         */
        template<typename component_type>
        void addComponentType() { addComponentType(typeHash<component_type>()); }


        template <class archetype, std::size_t N = std::tuple_size<archetype>::value,typename Indicies = std::make_index_sequence<N>>
        void addArchetype()
        {
            addArchetypeEnumerated<archetype>(Indicies{});
        }

        template <class archetype,std::size_t... I>
        void addArchetypeEnumerated(std::index_sequence<I...>)
        {
            (addComponentType<std::tuple_element<I,archetype>>(),...);
        }

        /**@brief Add component type to query for.
         * @param componentTypeId
         */
        void addComponentType(id_type componentTypeId);

        /**@brief Remove component type to query for.
         * @tparam component_type
         */
        template<typename component_type>
        void removeComponentType() { removeComponentType(typeHash<component_type>()); }

        /**@brief Remove component type to query for.
         * @param componentTypeId
         */
        void removeComponentType(id_type componentTypeId);

        /**@brief Get entity handle at a certain index in the entity list.
         */
        entity_handle operator[](size_type index);

        /**@brief Get amount of entities that were found with the queried component types.
         */
        size_type size();
    };
}
