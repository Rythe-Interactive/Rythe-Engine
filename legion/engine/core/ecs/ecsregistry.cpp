#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/ecs/component_pool.hpp>
#include <core/ecs/component_handle.hpp>

#include <core/events/eventbus.hpp>

namespace legion::core::ecs
{
    // 2 because the world entity is 1 and 0 is invalid_id
    id_type EcsRegistry::m_nextEntityId = 2;
    entity_handle EcsRegistry::world = entity_handle(world_entity_id);

    void EcsRegistry::recursiveDestroyEntityInternal(id_type entityId)
    {
#ifdef LGN_SAFE_MODE
        if (!validateEntity(entityId))
            return;
#endif

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

    EcsRegistry::EcsRegistry(events::EventBus* eventBus) : m_families(), m_entityData(), m_entities(), m_queryRegistry(*this), m_eventBus(eventBus)
    {
        entity_handle::m_registry = this;
        entity_handle::m_eventBus = eventBus;
        // Create world entity.
        m_entityData.emplace(world_entity_id, entity_data());
        m_entities.emplace(world_entity_id);
    }

    component_pool_base* EcsRegistry::getFamily(id_type componentTypeId)
    {
        OPTICK_EVENT();
        async::readonly_guard guard(m_familyLock);
        return m_families.at(componentTypeId).get();
    }

    bool EcsRegistry::hasComponent(id_type entityId, id_type componentTypeId)
    {
        OPTICK_EVENT();
        return getFamily(componentTypeId)->has_component(entityId);
    }

    component_handle_base EcsRegistry::getComponent(id_type entityId, id_type componentTypeId)
    {
        OPTICK_EVENT();
        return component_handle_base(entityId, componentTypeId);
    }

    component_handle_base EcsRegistry::createComponent(id_type entityId, id_type componentTypeId)
    {
        OPTICK_EVENT();
#ifdef LGN_SAFE_MODE
        if (!validateEntity(entityId))
            return component_handle_base();
#endif

        getFamily(componentTypeId)->create_component(entityId);

        {
            async::readonly_guard guard(m_entityDataLock);
            m_entityData[entityId].components.insert(componentTypeId); // Is fine because the lock only locks order changes in the container, not the values themselves.
        }

        m_queryRegistry.evaluateEntityChange(entityId, componentTypeId, false);

        return component_handle_base(entityId, componentTypeId);
    }



    component_handle_base EcsRegistry::copyComponent(id_type destinationEntity, id_type sourceEntity, id_type componentTypeId)
    {
        OPTICK_EVENT();
#ifdef LGN_SAFE_MODE
        if (!validateEntity(sourceEntity) || !validateEntity(destinationEntity))
            return component_handle_base();
#endif

        getFamily(componentTypeId)->clone_component(destinationEntity, sourceEntity);

        {
            async::readonly_guard guard(m_entityDataLock);
            m_entityData[destinationEntity].components.insert(componentTypeId); // Is fine because the lock only locks order changes in the container, not the values themselves.
        }

        m_queryRegistry.evaluateEntityChange(destinationEntity, componentTypeId, false);

        return component_handle_base(destinationEntity, componentTypeId);
    }

    component_handle_base EcsRegistry::createComponent(id_type entityId, id_type componentTypeId, void* value)
    {
        OPTICK_EVENT();
#ifdef LGN_SAFE_MODE
        if (!validateEntity(entityId))
            return component_handle_base();
#endif

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
        OPTICK_EVENT();
#ifdef LGN_SAFE_MODE
        if (!validateEntity(entityId))
            return;
#endif

        m_queryRegistry.evaluateEntityChange(entityId, componentTypeId, true);
        getFamily(componentTypeId)->destroy_component(entityId);

        {
            async::readonly_guard guard(m_entityDataLock);
            m_entityData[entityId].components.erase(componentTypeId); // Is fine because the lock only locks order changes in the container, not the values themselves.
        }
    }

    L_NODISCARD bool EcsRegistry::validateEntity(id_type entityId)
    {
        OPTICK_EVENT();
        if (!entityId)
            return false;
        async::readonly_guard guard(m_entityLock);
        return m_entities.contains(entityId);
    }

    entity_handle EcsRegistry::createEntity(id_type entityId)
    {
        OPTICK_EVENT();
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

        entity_data data{};
        data.parent = world_entity_id;

        {
            async::readwrite_guard guard(m_entityDataLock);  // We need write permission now because we hope to insert a new item.
            m_entityData.emplace(id, std::move(data));
            m_entityData[world_entity_id].children.insert({ id });
        }

        async::readwrite_guard guard(m_entityLock); // No scope needed because we also need read permission in the return line.
        m_entities.emplace(id);

        return entity_handle(id);
    }

    void EcsRegistry::destroyEntity(id_type entityId, bool recurse)
    {
        OPTICK_EVENT();
#ifdef LGN_SAFE_MODE
        if (!validateEntity(entityId))
            return;
#endif

        m_queryRegistry.markEntityDestruction(entityId); // Remove entity from any queries.

        entity_handle entity(entityId);

        {
            async::readwrite_guard guard(m_entityLock); // Request read-write permission for the entity list.
            entity.set_parent(invalid_id); // Remove ourselves as child from parent.
            m_entities.erase(entity); // Erase the entity from the entity list first, invalidating the entity and stopping any other function from being called on this entity.
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
        OPTICK_EVENT();
#ifdef LGN_SAFE_MODE
        if (!validateEntity(entityId))
            return entity_handle(invalid_id);
#endif

        return entity_handle(entityId);;
    }

    L_NODISCARD entity_data EcsRegistry::getEntityData(id_type entityId)
    {
        OPTICK_EVENT();
#ifdef LGN_SAFE_MODE
        if (!validateEntity(entityId))
            return entity_data();
#endif

        async::readonly_guard guard(m_entityDataLock);
        entity_data& data = m_entityData[entityId]; // Is fine because the lock only locks order changes in the container, not the values themselves.

#ifdef LGN_SAFE_MODE
        if (data.parent && !validateEntity(data.parent)) // Re-validate parent.
            data.parent = invalid_id;
#endif

        return data;
    }

    void EcsRegistry::setEntityData(id_type entityId, const entity_data& data)
    {
        OPTICK_EVENT();
#ifdef LGN_SAFE_MODE
        if (!validateEntity(entityId))
            return;
#endif

        async::readonly_guard guard(m_entityDataLock);
        m_entityData[entityId] = data;
    }

    L_NODISCARD entity_handle EcsRegistry::getEntityParent(id_type entityId)
    {
        OPTICK_EVENT();
        id_type parentId;

        {
            async::readonly_guard guard(m_entityDataLock);
            parentId = m_entityData[entityId].parent;
        }

#ifdef LGN_SAFE_MODE
        if (parentId && !validateEntity(parentId)) // Re-validate parent.
            parentId = invalid_id;
#endif

        return { parentId };
    }

    L_NODISCARD std::pair<entity_set&, async::rw_spinlock&> EcsRegistry::getEntities()
    {
        OPTICK_EVENT();
        return std::make_pair(std::ref(m_entities), std::ref(m_entityLock));
    }
}
