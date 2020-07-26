#include <core/ecs/entityquery.hpp>
#include <core/ecs/queryregistry.hpp>
#include <core/ecs/entity.hpp>

namespace args::core::ecs
{
	void EntityQuery::addComponentType(id_type componentTypeId)
	{
		m_registry->addComponentType(m_id, componentTypeId);
	}
	
	void EntityQuery::removeComponentType(id_type componentTypeId)
	{
		m_registry->removeComponentType(m_id, componentTypeId);
	}
	
	entity EntityQuery::operator[](size_type index)
	{
		return entity();
	}
	
	size_type EntityQuery::size()
	{
		return size_type();
	}
}
