#include <core/ecs/queryregistry.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/ecs/entityquery.hpp>
#include <algorithm>

namespace args::core::ecs
{
	void QueryRegistry::addComponentType(id_type queryId, id_type componentTypeId)
	{
		m_componentTypes[queryId].insert(componentTypeId);

		sparse_map<id_type, entity_handle>& entities = m_registry.getEntities();
		for (id_type entityId : entities.keys())
		{
			entity_handle& entity_handle = entities[entityId];
			if (entity_handle.component_composition().contains(m_componentTypes[queryId]))
				m_entityLists[queryId].insert(entityId, entity_handle);
		}
	}

	void QueryRegistry::removeComponentType(id_type queryId, id_type componentTypeId)
	{
		m_componentTypes[queryId].erase(componentTypeId);

		std::vector<id_type> toRemove;
		for (int i = 0; i < m_entityLists[queryId].size(); i++)
		{
			id_type entityId = m_entityLists[queryId].dense()[i];
			if (!m_registry.getEntity(entityId).component_composition().contains(m_componentTypes[queryId]))
				toRemove.push_back(entityId);
		}

		for (id_type entityId : toRemove)
			m_entityLists[queryId].erase(entityId);
	}

	inline void QueryRegistry::evaluateEntityChange(id_type entityId, id_type componentTypeId, bool removal)
	{
		for (int i = 0; i < m_entityLists.size(); i++)
		{
			if (!m_componentTypes[i].contains(componentTypeId))
				continue;

			if (m_entityLists[i].contains(entityId))
			{
				if (removal)
				{
					m_entityLists[i].erase(entityId);
					continue;
				}
			}
			else
			{
				entity_handle entity_handle = m_registry.getEntity(entityId);
				if (entity_handle.component_composition().contains(m_componentTypes[i]))
					m_entityLists[i].insert(entityId, entity_handle);
			}
		}
	}

	inline void QueryRegistry::markEntityDestruction(id_type entityId)
	{
		for (int i = 0; i < m_entityLists.size(); i++)
			if (m_entityLists[i].contains(entityId))
				m_entityLists[i].erase(entityId);
	}

	inline id_type QueryRegistry::getQueryId(const hashed_sparse_set<id_type>& componentTypes)
	{
		for (int id : m_componentTypes.keys())
		{
			if (m_componentTypes[id] == componentTypes)
				return id;
		}

		return invalid_id;
	}

	inline EntityQuery QueryRegistry::createQuery(const hashed_sparse_set<id_type>& componentTypes)
	{
		id_type queryId = getQueryId(componentTypes);

		if (!queryId)
		{
			queryId = addQuery(componentTypes);
		}

		return EntityQuery(queryId, *this, m_registry);
	}

	inline const hashed_sparse_set<id_type>& QueryRegistry::getComponentTypes(id_type queryId)
	{
		return m_componentTypes[queryId];
	}

	id_type QueryRegistry::addQuery(const hashed_sparse_set<id_type>& componentTypes)
	{
		id_type queryId = m_entityLists.size() + 1;
		m_entityLists.emplace(queryId);
		m_references.insert(queryId, 1);

		hashed_sparse_set<id_type> componentMap;
		for (id_type componentTypeId : componentTypes)
			componentMap.insert(componentTypeId);

		m_componentTypes.insert(queryId, componentMap);

		for (entity_handle& entity_handle : m_registry.getEntities())
			if (entity_handle.component_composition().contains(m_componentTypes[queryId]))
				m_entityLists[queryId].insert(entity_handle, entity_handle);

		return queryId;
	}

	inline const sparse_map<id_type, entity_handle>& QueryRegistry::getEntities(id_type queryId)
	{
		return m_entityLists.get(queryId);
	}

	inline void QueryRegistry::addReference(id_type queryId)
	{
		m_references.get(queryId)++;
	}

	inline void QueryRegistry::removeReference(id_type queryId)
	{
		if (queryId == invalid_id || !m_references.contains(queryId))
			return;

		size_type& referenceCount = m_references.get(queryId);
		referenceCount--;
		if (referenceCount == 0)
		{
			m_references.erase(queryId);
			m_entityLists.erase(queryId);
			m_componentTypes.erase(queryId);
		}
	}

	inline size_type QueryRegistry::getReferenceCount(id_type queryId)
	{
		if (m_references.contains(queryId))
			return m_references[queryId];
		return 0;
	}
}