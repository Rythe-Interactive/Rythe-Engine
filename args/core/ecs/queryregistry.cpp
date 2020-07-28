#include <core/ecs/queryregistry.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity.hpp>
#include <core/ecs/entityquery.hpp>
#include <algorithm>

namespace args::core::ecs
{
	void QueryRegistry::addComponentType(id_type queryId, id_type componentTypeId)
	{
		m_componentTypes[queryId].insert(componentTypeId, componentTypeId);

		std::vector<entity>& entities = m_registry.getEntities();
		for (int i = 0; i < entities.size(); i++)
		{
			if (entities[i].component_composition().contains(m_componentTypes[queryId]))
				m_entityLists[queryId].insert(entities[i], entities[i]);
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
				entity entity = m_registry.getEntity(entityId);
				if (entity.component_composition().contains(m_componentTypes[i]))
					m_entityLists[i].insert(entityId, entity);
			}
		}
	}

	inline void QueryRegistry::markEntityDestruction(id_type entityId)
	{
		for (int i = 0; i < m_entityLists.size(); i++)
			if (m_entityLists[i].contains(entityId))
				m_entityLists[i].erase(entityId);
	}

	inline id_type QueryRegistry::getQueryId(const sparse_map<id_type, id_type>::dense_value_container& componentTypes)
	{
		for (int id  : m_componentTypes.keys())
		{
			if (m_componentTypes[id].dense() == componentTypes)
				return id;
		}

		return invalid_id;
	}

	inline EntityQuery QueryRegistry::createQuery(const sparse_map<id_type, id_type>::dense_value_container& componentTypes)
	{
		id_type queryId = getQueryId(componentTypes);

		if (!queryId)
		{
			queryId = addQuery(componentTypes);
		}

		return EntityQuery(queryId, *this, m_registry);
	}

	inline sparse_map<id_type, id_type> QueryRegistry::getComponentTypes(id_type queryId)
	{
		return m_componentTypes[queryId];
	}

	id_type QueryRegistry::addQuery(const sparse_map<id_type, id_type>::dense_value_container& componentTypes)
	{
		id_type queryId = m_entityLists.size();
		m_entityLists.emplace(queryId);
		m_references.insert(queryId, 1);

		sparse_map<id_type, id_type> componentMap;
		for (id_type componentTypeId : componentTypes)
			componentMap.insert(componentTypeId, componentTypeId);

		m_componentTypes.insert(queryId, componentMap);

		auto& entities = m_registry.getEntities();
		for (int i = 0; i < entities.size(); i++)
			if (entities[i].component_composition().contains(m_componentTypes[queryId]))
				m_entityLists[queryId].insert(entities[i], entities[i]);

		return queryId;
	}

	inline sparse_map<id_type, entity>& QueryRegistry::getEntities(id_type queryId)
	{
		return m_entityLists.get(queryId);
	}

	inline void QueryRegistry::addReference(id_type queryId)
	{
		m_references.get(queryId)++;
	}

	inline void QueryRegistry::removeReference(id_type queryId)
	{
		if (!m_references.contains(queryId))
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