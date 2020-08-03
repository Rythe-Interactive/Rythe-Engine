#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/ecs/component_container.hpp>
#include <core/ecs/component_handle.hpp>

namespace args::core::ecs
{
	// 2 because the world entity is 1 and 0 is invalid_id
	id_type EcsRegistry::m_nextEntityId = 2;

	void EcsRegistry::recursiveDestroyEntityInternal(id_type entityId)
	{
		if (!validateEntity(entityId))
			throw args_entity_not_found_error;

		// If you wonder why we don't remove our parent, our parent is the one calling this function so it'll be destroyed anyways.

		{
			async::readwrite_guard guard(m_entityLock); // Request read-write permission for the entity list.
			m_entities.erase(entityId); // Erase the entity from the entity list first, invalidating the entity and stopping any other function from being called on this entity.
		}

		entity_data data = {};

		{
			async::readwrite_guard guard(m_entityDataLock); // Request read-write permission for the entity data list.
			data = std::move(m_entityData[entityId]); // Fetch data of entity to destroy.
			m_entityData.erase(entityId); // We can also safely erase the data since the entity has already been invalidated.
		}

		{
			async::readonly_guard guard(m_familyLock); // Technically possibly deadlocks. However the only write op on families happen when creating the family. Will also lock atomic_sparse_map::m_container_lock for the family.
			for (id_type componentTypeId : data.components) // Destroy all components attached to this entity.
			{
				m_families[componentTypeId]->destroy_component(entityId);
			}
		}

		for (entity_handle& child : data.children)	//
			recursiveDestroyEntityInternal(child);

		m_queryRegistry.markEntityDestruction(entityId);
	}

	EcsRegistry::EcsRegistry() : m_families(), m_entityData(), m_entities(), m_queryRegistry(*this)
	{
		// Create world entity.
		m_entityData.emplace(1);
		m_entities.emplace(1, 1, this);
	}

	inline component_container_base* EcsRegistry::getFamily(id_type componentTypeId)
	{
		async::readonly_guard guard(m_familyLock);

		if (!m_families.contains(componentTypeId))
			throw args_unknown_component_error;

		return m_families[componentTypeId].get();
	}

	inline component_handle_base EcsRegistry::getComponent(id_type entityId, id_type componentTypeId)
	{
		if (!validateEntity(entityId))
			throw args_entity_not_found_error;

		return component_handle_base(entityId, *this);
	}

	inline component_handle_base EcsRegistry::createComponent(id_type entityId, id_type componentTypeId)
	{
		if (!validateEntity(entityId))
			throw args_entity_not_found_error;

		getFamily(componentTypeId)->create_component(entityId);

		{
			async::readonly_guard guard(m_entityDataLock);
			m_entityData[entityId].components.insert(componentTypeId);
		}

		m_queryRegistry.evaluateEntityChange(entityId, componentTypeId, true);

		return component_handle_base(entityId, *this);
	}

	inline void EcsRegistry::destroyComponent(id_type entityId, id_type componentTypeId)
	{
		if (!validateEntity(entityId))
			throw args_entity_not_found_error;

		getFamily(componentTypeId)->destroy_component(entityId);

		{
			async::readonly_guard guard(m_entityDataLock);
			m_entityData[entityId].components.erase(componentTypeId);
		}

		m_queryRegistry.evaluateEntityChange(entityId, componentTypeId, true);
	}

	A_NODISCARD inline bool EcsRegistry::validateEntity(id_type entityId)
	{
		async::readonly_guard guard(m_entityLock);
		return entityId && m_entities.contains(entityId);
	}

	inline entity_handle EcsRegistry::createEntity()
	{
		id_type id = m_nextEntityId++;

		if (validateEntity(id))
			throw args_entity_exists_error;

		{
			async::readwrite_guard guard(m_entityDataLock);
			m_entityData[id] = {};
		}

		async::readwrite_guard guard(m_entityLock);
		m_entities.emplace(id, id, this);

		return m_entities[id];
	}

	inline void EcsRegistry::destroyEntity(id_type entityId, bool recurse)
	{
		if (!validateEntity(entityId))
			throw args_entity_not_found_error;

		entity_data* data = nullptr;

		{
			async::readonly_guard guard(m_entityDataLock);
			data = &m_entityData[entityId];
		}

		{
			async::readonly_guard guard(m_familyLock); // Technically possibly deadlocks. However the only write op on families happen when creating the family. Will also lock atomic_sparse_map::m_container_lock for the family.
			for (id_type componentTypeId : data->components)
			{
				m_families[componentTypeId]->destroy_component(entityId);
			}
		}

		{
			async::readonly_guard guard(m_entityLock);
			m_entities[entityId].set_parent(invalid_id);
		}

		for (entity_handle& child : data->children)
			if (recurse)
				recursiveDestroyEntityInternal(child);
			else
				child.set_parent(invalid_id);

		{
			async::readwrite_guard guard(m_entityDataLock);
			m_entityData.erase(entityId);
		}
		{
			async::readwrite_guard guard(m_entityLock);
			m_entities.erase(entityId);
		}

		m_queryRegistry.markEntityDestruction(entityId);
	}

	A_NODISCARD inline entity_handle EcsRegistry::getEntity(id_type entityId)
	{
		if (!validateEntity(entityId))
			return entity_handle(invalid_id, this);

		async::readonly_guard guard(m_entityLock);
		return m_entities[entityId];
	}

	A_NODISCARD inline entity_data& EcsRegistry::getEntityData(id_type entityId)
	{
		if (!validateEntity(entityId))
			throw args_entity_not_found_error;

		entity_data* data = nullptr;

		{
			async::readonly_guard guard(m_entityDataLock);
			data = &m_entityData[entityId];
		}

		if (data->parent && !validateEntity(data->parent))
			data->parent = invalid_id;

		return *data;
	}

	A_NODISCARD inline std::pair<sparse_map<id_type, entity_handle>&, async::readonly_rw_spinlock&> EcsRegistry::getEntities()
	{
		return std::pair<sparse_map<id_type, entity_handle>&, async::readonly_rw_spinlock&>(m_entities, m_entityLock);
	}
}
