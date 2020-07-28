#pragma once
#include <vector>
#include <core/containers/containers.hpp>
#include <core/types/primitives.hpp>

namespace args::core::ecs
{
	class ARGS_API EcsRegistry;
	class ARGS_API entity_handle;
	class ARGS_API EntityQuery;

	class ARGS_API QueryRegistry
	{
	private:
		EcsRegistry& m_registry;
		sparse_map<id_type, sparse_map<id_type, entity_handle>> m_entityLists;
		sparse_map<id_type, size_type> m_references;
		sparse_map<id_type, sparse_map<id_type, id_type>> m_componentTypes;

	public:
		QueryRegistry(EcsRegistry& registry) : m_registry(registry), m_entityLists(), m_componentTypes() {}

		void addComponentType(id_type queryId, id_type componentTypeId);

		void removeComponentType(id_type queryId, id_type componentTypeId);

		void evaluateEntityChange(id_type entityId, id_type componentTypeId, bool removal);

		void markEntityDestruction(id_type entityId);

		id_type getQueryId(const sparse_map<id_type, id_type>::dense_value_container& componentTypes);

		template<typename... component_types>
		EntityQuery createQuery()
		{
			std::vector<id_type> componentTypeIds;
			(componentTypeIds.push_back(typeHash<component_types>()), ...);
			return createQuery(componentTypeIds);
		}

		EntityQuery createQuery(const sparse_map<id_type, id_type>::dense_value_container& componentTypes);

		sparse_map<id_type, id_type> getComponentTypes(id_type queryId);

		id_type addQuery(const sparse_map<id_type, id_type>::dense_value_container& componentTypes);

		sparse_map<id_type, entity_handle>& getEntities(id_type queryId);

		void addReference(id_type queryId);
		void removeReference(id_type queryId);
		size_type getReferenceCount(id_type queryId);
	};

}