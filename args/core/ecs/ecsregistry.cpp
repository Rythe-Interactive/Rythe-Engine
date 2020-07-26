#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity.hpp>
#include <core/ecs/component_container.hpp>
#include <core/ecs/component_handle.hpp>

namespace args::core::ecs
{
	// 2 because the world entity is 1 and 0 is invalid_id
	id_type EcsRegistry::lastEntityId = 2;

	EcsRegistry::EcsRegistry() : families(), entities()
	{
		// Create world entity.
		entities[1] = new entity(1, *this);
	}

	inline component_container_base* EcsRegistry::getFamily(id_type componentTypeId)
	{
		if (!families.count(componentTypeId))
			throw args_unknown_component_error;

		return families[componentTypeId];
	}

	inline component_handle_base EcsRegistry::getComponent(id_type entityId, id_type componentTypeId)
	{
		if (getEntity(entityId).has_component(componentTypeId))
			return component_handle_base(entityId, *this);
		return component_handle_base(invalid_id, *this);
	}

	inline component_handle_base EcsRegistry::createComponent(id_type entityId, id_type componentTypeId)
	{
		entity& entity = getEntity(entityId);

		getFamily(componentTypeId)->create_component(entityId);

		entity.m_components.insert(componentTypeId, componentTypeId);

		return component_handle_base(entityId, *this);
	}

	void EcsRegistry::destroyComponent(id_type entityId, id_type componentTypeId)
	{
		entity& entity = getEntity(entityId);

		getFamily(componentTypeId)->destroy_component(entityId);

		entity.m_components.erase(componentTypeId);
	}

	inline entity& EcsRegistry::createEntity()
	{
		id_type id = lastEntityId++;

		if (entities.count(id))
			throw args_entity_exists_error;

		entities[id] = new entity(id, *this);

		return *entities[id];
	}

	inline void EcsRegistry::destroyEntity(id_type entityId)
	{
		delete entities[entityId];
		entities.erase(entityId);
	}

	inline entity& EcsRegistry::getEntity(id_type entityId)
	{
		if (!entities.count(entityId))
			throw args_entity_not_found_error;

		return *entities[entityId];
	}
}