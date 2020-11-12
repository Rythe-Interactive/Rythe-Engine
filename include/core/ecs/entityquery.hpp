#pragma once
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/hashed_sparse_set.hpp>
#include <core/ecs/entity_handle.hpp>

/**
 * @file entityquery.hpp
 */

namespace legion::core::ecs
{
	class QueryRegistry;
	class EcsRegistry;

	/**@class EntityQuery
	 * @brief Handle to an entity query. Allows you to acquire a list all of entities with a certain component combination.
	 */
	class EntityQuery
	{
	private:
		QueryRegistry* m_registry;
		EcsRegistry* m_ecsRegistry;
		id_type m_id;

	public:
		EntityQuery(id_type id, QueryRegistry* registry, EcsRegistry* ecsRegistry);
        EntityQuery() = default;
		~EntityQuery();

        EntityQuery(EntityQuery&& other);
        EntityQuery(const EntityQuery& other);

        EntityQuery operator=(EntityQuery&& other);
        EntityQuery operator=(const EntityQuery& other);

		/**@brief Get begin iterator for entity handles to the queried entities.
		 */
        entity_set::const_iterator begin() const;

		/**@brief Get end iterator for entity handles to the queried entities.
		 */
        entity_set::const_iterator end() const;

		/**@brief Get query id.
		 */
		L_NODISCARD id_type id() { return m_id; }

		/**@brief Add component type to query for.
		 * @tparam component_type
		 */
		template<typename component_type>
		void addComponentType() { addComponentType(typeHash<component_type>()); }

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
