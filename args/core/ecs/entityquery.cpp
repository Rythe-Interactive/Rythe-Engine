#include <core/ecs/entityquery.hpp>
#include <core/ecs/queryregistry.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity_handle.hpp>

namespace args::core::ecs
{
	EntityQuery::~EntityQuery()
	{
		m_registry.removeReference(m_id);
	}

	sparse_map<id_type, entity_handle>::iterator EntityQuery::begin()
	{
		return m_registry.getEntities(m_id).begin();
	}

	sparse_map<id_type, entity_handle>::const_iterator EntityQuery::begin() const
	{
		return m_registry.getEntities(m_id).begin();
	}

	sparse_map<id_type, entity_handle>::iterator EntityQuery::end()
	{
		return m_registry.getEntities(m_id).end();
	}

	sparse_map<id_type, entity_handle>::const_iterator EntityQuery::end() const
	{
		return m_registry.getEntities(m_id).end();
	}

	inline void EntityQuery::addComponentType(id_type componentTypeId)
	{
		std::vector<id_type> componentTypes;
		if (m_id)
			componentTypes = m_registry.getComponentTypes(m_id).dense();

		componentTypes.push_back(componentTypeId);

		id_type newId = m_registry.getQueryId(componentTypes);
		if (newId)
		{
			m_registry.removeReference(m_id);
			m_id = newId;
		}
		else if (m_id == invalid_id || m_registry.getReferenceCount(m_id) > 1)
			m_id = m_registry.addQuery(componentTypes);
		else
			m_registry.addComponentType(m_id, componentTypeId);
	}

	inline void EntityQuery::removeComponentType(id_type componentTypeId)
	{
		sparse_map<id_type, id_type> componentMap = m_registry.getComponentTypes(m_id);
		componentMap.erase(componentTypeId);
		std::vector<id_type>& componentTypes = componentMap.dense();

		id_type newId = m_registry.getQueryId(componentTypes);
		if (newId)
		{
			m_registry.removeReference(m_id);
			m_id = newId;
		}
		else if (m_id == invalid_id || m_registry.getReferenceCount(m_id) > 1)
			m_id = m_registry.addQuery(componentTypes);
		else
			m_registry.removeComponentType(m_id, componentTypeId);
	}

	inline entity_handle EntityQuery::operator[](size_type index)
	{
		return m_ecsRegistry.getEntity(m_registry.getEntities(m_id).dense()[index]);
	}

	inline size_type EntityQuery::size()
	{
		return m_registry.getEntities(m_id).size();
	}
}
