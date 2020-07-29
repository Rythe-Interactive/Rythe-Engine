#pragma once
#include <vector>
#include <core/containers/containers.hpp>
#include <core/types/primitives.hpp>

/**
 * @file queryregistry.hpp
 */

namespace args::core::ecs
{
	class ARGS_API EcsRegistry;
	class ARGS_API entity_handle;
	class ARGS_API EntityQuery;

	/**@class QueryRegistry
	 * @brief Main manager and owner of all queries and query related objects.
	 */
	class ARGS_API QueryRegistry
	{
	private:
		EcsRegistry& m_registry;
		sparse_map<id_type, sparse_map<id_type, entity_handle>> m_entityLists;
		sparse_map<id_type, size_type> m_references;
		sparse_map<id_type, sparse_map<id_type, id_type>> m_componentTypes;

	public:
		/**@brief Constructor, initializes all data.
		 */
		QueryRegistry(EcsRegistry& registry) : m_registry(registry), m_entityLists(), m_componentTypes() {}

		/**@brief Add a certain component type to a query.
		 * @warning Changing the components for this query id will also change it for any other references that may not want the same change.
		 *			For safety first check the amount of references to this query id and only change it if there's only 1. otherwise generate a new query.
		 * @warning A query with the components you want might already exist, adding them to this query might create duplicates.
		 *			For safety and efficiency first check if there isn't another query already using getQueryId.
		 * @note Changes components to query for all other queries that used to query the same components.
		 * @note Not the fastest thing in the world, don't do it with loads of components. In that case create a new query instead.
		 */
		void addComponentType(id_type queryId, id_type componentTypeId);

		void removeComponentType(id_type queryId, id_type componentTypeId);

		void evaluateEntityChange(id_type entityId, id_type componentTypeId, bool removal);

		void markEntityDestruction(id_type entityId);

		id_type getQueryId(const sparse_map<id_type, id_type>& componentTypes);

		template<typename... component_types>
		EntityQuery createQuery()
		{
			sparse_map<id_type, id_type> componentTypeIds;
			(componentTypeIds.insert(typeHash<component_types>(), typeHash<component_types>()), ...);
			return createQuery(componentTypeIds);
		}

		EntityQuery createQuery(const sparse_map<id_type, id_type>& componentTypes);

		sparse_map<id_type, id_type> getComponentTypes(id_type queryId);

		id_type addQuery(const sparse_map<id_type, id_type>& componentTypes);

		sparse_map<id_type, entity_handle>& getEntities(id_type queryId);

		void addReference(id_type queryId);
		void removeReference(id_type queryId);
		size_type getReferenceCount(id_type queryId);
	};

}