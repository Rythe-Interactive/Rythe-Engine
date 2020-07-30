#include <core/ecs/queryregistry.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/ecs/entityquery.hpp>
#include <algorithm>
#include <mutex>

namespace args::core::ecs
{
	void QueryRegistry::addComponentType(id_type queryId, id_type componentTypeId)
	{
		{
			async::readwrite_guard guard(m_componentLock);
			m_componentTypes[queryId].insert(componentTypeId);
		}

		auto entityData = m_registry.getEntities();
		sparse_map<id_type, entity_handle>& entities = entityData.first;
		async::readonly_multiguard mguard(entityData.second, m_componentLock);

		for (id_type entityId : entities.keys())
		{
			entity_handle& entity_handle = entities[entityId];
			if (entity_handle.component_composition().contains(m_componentTypes[queryId]))
				m_entityLists[queryId].insert(entityId, entity_handle);
		}
	}

	void QueryRegistry::removeComponentType(id_type queryId, id_type componentTypeId)
	{
		{
			async::readwrite_guard guard(m_componentLock);
			m_componentTypes[queryId].erase(componentTypeId);
		}

		
		std::vector<id_type> toRemove;

		{
			async::readonly_multiguard mguard(m_entityLock, m_componentLock);
			for (int i = 0; i < m_entityLists[queryId].size(); i++)
			{
				id_type entityId = m_entityLists[queryId].dense()[i];
				if (!m_registry.getEntity(entityId).component_composition().contains(m_componentTypes[queryId]))
					toRemove.push_back(entityId);
			}
		}

		{
			async::readwrite_guard guard(m_entityLock);
			for (id_type entityId : toRemove)
				m_entityLists[queryId].erase(entityId);
		}
	}

	inline void QueryRegistry::evaluateEntityChange(id_type entityId, id_type componentTypeId, bool removal)
	{
		async::read_state entitystate = async::write;
		async::read_state compstate = async::read;
		async::mixed_multiguard mmguard(m_entityLock, entitystate, m_componentLock, compstate);

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
		async::readwrite_guard guard(m_entityLock);
		for (int i = 0; i < m_entityLists.size(); i++)
			if (m_entityLists[i].contains(entityId))
				m_entityLists[i].erase(entityId);
	}

	inline id_type QueryRegistry::getQueryId(const hashed_sparse_set<id_type>& componentTypes)
	{
		async::readonly_guard guard(m_componentLock);

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
		async::readonly_guard guard(m_componentLock);
		return m_componentTypes[queryId];
	}

	id_type QueryRegistry::addQuery(const hashed_sparse_set<id_type>& componentTypes)
	{
		id_type queryId;

		{ // Write permitted critical section for m_entityLists
			async::readwrite_guard entguard(m_entityLock);

			queryId = m_entityLists.size() + 1;
			m_entityLists.emplace(queryId);
		}

		{
			async::readwrite_guard refguard(m_referenceLock);
			m_references.insert(queryId, 1);
		}

		hashed_sparse_set<id_type> componentMap;
		for (id_type componentTypeId : componentTypes)
			componentMap.insert(componentTypeId);

		{
			async::readwrite_guard compguard(m_componentLock);
			m_componentTypes.insert(queryId, componentMap);
		}

		{
			auto entityData = m_registry.getEntities();
			async::readonly_guard dataguard(entityData.second);
			async::readwrite_guard entguard(m_entityLock);
			async::readonly_guard compguard(m_componentLock);

			for (entity_handle& entity_handle : entityData.first)
				if (entity_handle.component_composition().contains(m_componentTypes[queryId]))
					m_entityLists[queryId].insert(entity_handle, entity_handle);
		}

		return queryId;
	}

	inline const sparse_map<id_type, entity_handle>& QueryRegistry::getEntities(id_type queryId)
	{
		async::readonly_guard entguard(m_entityLock);
		return m_entityLists.get(queryId);
	}

	inline void QueryRegistry::addReference(id_type queryId)
	{
		async::readonly_guard refguard(m_referenceLock);
		m_references.get(queryId)++;
	}

	inline void QueryRegistry::removeReference(id_type queryId)
	{
		async::readonly_guard refguard(m_referenceLock);

		if (queryId == invalid_id || !m_references.contains(queryId))
			return;

		size_type& referenceCount = m_references.get(queryId);
		referenceCount--;
		if (referenceCount == 0)
		{
			async::readwrite_guard refWguard(m_referenceLock); // permitted to double lock, elevates permission to write.
			async::readwrite_guard entguard(m_entityLock);
			async::readwrite_guard compguard(m_componentLock);

			m_references.erase(queryId);
			m_entityLists.erase(queryId);
			m_componentTypes.erase(queryId);
		}
	}

	inline size_type QueryRegistry::getReferenceCount(id_type queryId)
	{
		async::readonly_guard refguard(m_referenceLock);
		if (m_references.contains(queryId))
			return m_references[queryId];
		return 0;
	}
}