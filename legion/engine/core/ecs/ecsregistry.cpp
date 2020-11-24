#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/ecs/component_container.hpp>
#include <core/ecs/component_handle.hpp>

#include <core/events/eventbus.hpp>

namespace legion::core::ecs
{
    // 2 because the world entity is 1 and 0 is invalid_id
    id_type EcsRegistry::m_nextEntityId = 2;

    void EcsRegistry::recursiveDestroyEntityInternal(id_type entityId)
    {
        if (!validateEntity(entityId))
            throw legion_entity_not_found_error;

        m_queryRegistry.markEntityDestruction(entityId); // Remove entity from any queries.

        {
            async::readwrite_guard guard(m_entityLock); // Request read-write permission for the entity list.
            // If you wonder why we don't remove our parent, our parent is the one calling this function so it'll be destroyed anyways.
            m_entities.erase(entity_handle(entityId)); // Erase the entity from the entity list first, invalidating the entity and stopping any other function from being called on this entity.
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

        for (entity_handle& child : data.children)	// Destroy all children.
            recursiveDestroyEntityInternal(child);
    }

    EcsRegistry::EcsRegistry(events::EventBus* eventBus) : m_families(), m_entityData(), m_containedEntities(), m_entities(), m_queryRegistry(*this), m_eventBus(eventBus)
    {
        entity_handle::m_registry = this;
        // Create world entity.
        m_entityData.emplace(world_entity_id);
        m_containedEntities.insert(world_entity_id);
        m_entities.emplace(world_entity_id);
        world = entity_handle(world_entity_id);
    }

    component_container_base* EcsRegistry::getFamily(id_type componentTypeId)
    {
        async::readonly_guard guard(m_familyLock);

        if (!m_families.contains(componentTypeId))
            throw legion_unknown_component_error;

        return m_families[componentTypeId].get();
    }

    bool EcsRegistry::hasComponent(id_type entityId, id_type componentTypeId)
    {
        if (!validateEntity(entityId))
            throw legion_entity_not_found_error;

        return getFamily(componentTypeId)->has_component(entityId);
    }

    component_handle_base EcsRegistry::getComponent(id_type entityId, id_type componentTypeId)
    {
        if (!validateEntity(entityId))
            throw legion_entity_not_found_error;

        return component_handle_base(entityId, componentTypeId);
    }

    component_handle_base EcsRegistry::createComponent(id_type entityId, id_type componentTypeId)
    {
        if (!validateEntity(entityId))
            throw legion_entity_not_found_error;

        getFamily(componentTypeId)->create_component(entityId);

        {
            async::readonly_guard guard(m_entityDataLock);
            m_entityData[entityId].components.insert(componentTypeId); // Is fine because the lock only locks order changes in the container, not the values themselves.
        }

        m_queryRegistry.evaluateEntityChange(entityId, componentTypeId, false);

        return component_handle_base(entityId, componentTypeId);
    }

    component_handle_base EcsRegistry::createComponent(id_type entityId, id_type componentTypeId, void* value)
    {
        if (!validateEntity(entityId))
            throw legion_entity_not_found_error;

        getFamily(componentTypeId)->create_component(entityId, value);

        {
            async::readonly_guard guard(m_entityDataLock);
            m_entityData[entityId].components.insert(componentTypeId); // Is fine because the lock only locks order changes in the container, not the values themselves.
        }

        m_queryRegistry.evaluateEntityChange(entityId, componentTypeId, false);

        return component_handle_base(entityId, componentTypeId);
    }

    void EcsRegistry::destroyComponent(id_type entityId, id_type componentTypeId)
    {
        if (!validateEntity(entityId))
            throw legion_entity_not_found_error;

        m_queryRegistry.evaluateEntityChange(entityId, componentTypeId, true);
        getFamily(componentTypeId)->destroy_component(entityId);

        {
            async::readonly_guard guard(m_entityDataLock);
            m_entityData[entityId].components.erase(componentTypeId); // Is fine because the lock only locks order changes in the container, not the values themselves.
        }
    }

    L_NODISCARD   bool EcsRegistry::validateEntity(id_type entityId)
    {
        async::readonly_guard guard(m_entityLock);
        return entityId && m_containedEntities.contains(entityId);
    }

    entity_handle EcsRegistry::createEntity(id_type entityId)
    {
        id_type id;
        if (!entityId)
            id = m_nextEntityId++;
        else
        {
            id = entityId;
            m_nextEntityId = entityId + 1;
        }

        if (validateEntity(id))
            return createEntity();

        {
            async::readwrite_guard guard(m_entityDataLock);  // We need write permission now because we hope to insert a new item.
            m_entityData[id] = entity_data{};
        }

        async::readwrite_guard guard(m_entityLock); // No scope needed because we also need read permission in the return line.
        m_entities.emplace(id);
        m_containedEntities.insert(id);

        entity_handle(id).set_parent(world_entity_id);

        return entity_handle(id);
    }

    void EcsRegistry::destroyEntity(id_type entityId, bool recurse)
    {
        if (!validateEntity(entityId))
            throw legion_entity_not_found_error;

        m_queryRegistry.markEntityDestruction(entityId); // Remove entity from any queries.

        entity_handle entity(entityId);

        for (auto child : entity)
        {

        }

        {
            async::readwrite_guard guard(m_entityLock); // Request read-write permission for the entity list.
            entity.set_parent(invalid_id); // Remove ourselves as child from parent.
            m_entities.erase(entity); // Erase the entity from the entity list first, invalidating the entity and stopping any other function from being called on this entity.
            m_containedEntities.erase(entityId);
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

        for (entity_handle& child : data.children)
            if (recurse)
                recursiveDestroyEntityInternal(child); // Recursively destroy all children
            else
                child.set_parent(invalid_id); // Remove parent from children.
    }

    L_NODISCARD entity_handle EcsRegistry::getEntity(id_type entityId)
    {
        if (!validateEntity(entityId))
            return entity_handle(invalid_id);

        return entity_handle(entityId);;
    }

    L_NODISCARD entity_data& EcsRegistry::getEntityData(id_type entityId)
    {
        if (!validateEntity(entityId))
            throw legion_entity_not_found_error;


        async::readonly_guard guard(m_entityDataLock);
        entity_data& data = m_entityData[entityId]; // Is fine because the lock only locks order changes in the container, not the values themselves.


        if (data.parent && !validateEntity(data.parent)) // Re-validate parent.
            data.parent = invalid_id;

        return data;
    }

    L_NODISCARD std::pair<entity_set&, async::readonly_rw_spinlock&> EcsRegistry::getEntities()
    {
        return std::make_pair(std::ref(m_entities), std::ref(m_entityLock));
    }
}
