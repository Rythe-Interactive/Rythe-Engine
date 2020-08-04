#pragma once
#include <core/types/primitives.hpp>
#include <core/types/type_util.hpp>
#include <core/platform/platform.hpp>
#include <core/containers/sparse_map.hpp>

/**
 * @file entityquery.hpp
 */

namespace args::core::ecs
{
	class ARGS_API QueryRegistry;
	class ARGS_API EcsRegistry;
	class ARGS_API entity_handle;

	/**@class EntityQuery
	 * @brief Handle to an entity query. Allows you to acquire a list all of entities with a certain component combination.
	 */
	class ARGS_API EntityQuery
	{
	private:
		QueryRegistry& m_registry;
		EcsRegistry& m_ecsRegistry;
		id_type m_id;

	public:
		EntityQuery(id_type id, QueryRegistry& registry, EcsRegistry& ecsRegistry) : m_registry(registry), m_ecsRegistry(ecsRegistry), m_id(id) { m_registry.addReference(m_id); }
		~EntityQuery() { m_registry.removeReference(m_id); }

		/**@brief Get begin iterator for entity handles to the queried entities.
		 */
		sparse_map<id_type, entity_handle>::const_iterator begin() const;

		/**@brief Get end iterator for entity handles to the queried entities.
		 */
		sparse_map<id_type, entity_handle>::const_iterator end() const;

		/**@brief Get query id.
		 */
		A_NODISCARD id_type id() { return m_id; }

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