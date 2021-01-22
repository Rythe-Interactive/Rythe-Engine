#include <core/ecs/queryregistry.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/entity_handle.hpp>
#include <core/ecs/entityquery.hpp>
#include <algorithm>

namespace legion::core::ecs
{
    hashed_sparse_set<QueryRegistry*> QueryRegistry::m_validRegistries;

    thread_local std::unordered_map<id_type, std::pair<float, entity_container>> QueryRegistry::m_localCopies;
    thread_local std::unordered_map<id_type, std::unordered_map<id_type, std::unique_ptr<component_container_base>>> QueryRegistry::m_localComponents;
    time::clock<fast_time> QueryRegistry::m_clock;

    void QueryRegistry::addComponentType(id_type queryId, id_type componentTypeId)
    {
        OPTICK_EVENT();
        {
            async::readwrite_guard guard(m_componentLock); // In this case the lock handles both the sparse_map and the contained hashed_sparse_sets
            m_componentTypes.at(queryId).insert(componentTypeId); // We insert the new component type we wish to track.
        }

        bool modified = false;

        // First we need to erase all the entities that no longer apply to the new query.
        std::vector<entity_handle> toRemove;

        {
            async::readonly_multiguard mguard(m_entityLock, m_componentLock);
            auto& [_, entityList] = m_entityLists.at(queryId);
            for (int i = 0; i < entityList.size(); i++) // Iterate over all tracked entities.
            {
                entity_handle entity = entityList.at(i); // Get the id from the keys of the map.
                if (!m_registry.getEntityData(entity).components.contains(m_componentTypes[queryId])) // Check component composition
                    toRemove.push_back(entity); // Mark for erasure if the component composition doesn't overlap with the query.
            }
        }

        if (toRemove.size() > 0)
        {
            modified = true;
            async::readwrite_guard guard(m_entityLock);
            auto& [_, entityList] = m_entityLists.at(queryId);
            for (entity_handle entity : toRemove)
                entityList.erase(entity); // Erase all entities marked for erasure.
        }

        // Next we need to filter through all the entities to get all the new ones that apply to the new query.

        auto [entities, entitiesLock] = m_registry.getEntities(); // getEntities returns a pair of both the container as well as the lock that should be locked by you when operating on it.
        async::mixed_multiguard mguard(entitiesLock, async::lock_state_read, m_componentLock, async::lock_state_read, m_entityLock, async::lock_state_write); // Lock locks.
        auto& [lastModified, entityList] = m_entityLists.at(queryId);
        for (entity_handle entity : entities) // Iterate over all entities.
        {
            if (entityList.contains(entity)) // If the entity is already tracked, continue to the next entity.
                continue;

            if (m_registry.getEntityData(entity).components.contains(m_componentTypes[queryId])) // Check if the queried components completely overlaps the components in the entity.
            {
                modified = true;
                entityList.insert(entity); // Insert entity into tracking list.
            }
        }

        if (modified)
            lastModified = m_clock.elapsedTime();
    }

    void QueryRegistry::removeComponentType(id_type queryId, id_type componentTypeId)
    {
        OPTICK_EVENT();
        {
            async::readwrite_guard guard(m_componentLock);
            m_componentTypes[queryId].erase(componentTypeId); // Remove component from query list.
        }

        // Then we remove all the entities that no longer overlap with the query.
        std::vector<entity_handle> toRemove;

        {
            async::readonly_multiguard mguard(m_entityLock, m_componentLock);
            auto& [_, entityList] = m_entityLists.at(queryId);
            for (int i = 0; i < entityList.size(); i++) // Iterate over all tracked entities.
            {
                entity_handle entity = entityList.at(i); // Get the id from the keys of the map.
                if (!m_registry.getEntity(entity).component_composition().contains(m_componentTypes[queryId])) // Check component composition
                    toRemove.push_back(entity); // Mark for erasure if the component composition doesn't overlap with the query.
            }
        }

        if (toRemove.size() > 0)
        {
            async::readwrite_guard guard(m_entityLock);
            auto& [lastModified, entityList] = m_entityLists.at(queryId);
            lastModified = m_clock.elapsedTime();
            for (entity_handle entity : toRemove)
                entityList.erase(entity); // Erase all entities marked for erasure.
        }
    }

    void QueryRegistry::evaluateEntityChange(id_type entityId, id_type componentTypeId, bool removal)
    {
        OPTICK_EVENT();
        entity_handle entity(entityId);

        async::mixed_multiguard mmguard(m_entityLock, async::lock_state_write, m_componentLock, async::lock_state_read); // We lock now so that we don't need to reacquire the locks every iteration.

        for (int i = 0; i < m_entityLists.size(); i++)
        {
            id_type queryId = m_entityLists.keys()[i];
            if (!m_componentTypes[queryId].contains(componentTypeId)) // This query doesn't care about this component type.
                continue;

            auto& [lastModified, entityList] = m_entityLists.at(queryId);
            if (entityList.contains(entity))
            {
                if (removal)
                {
                    entityList.erase(entity); // Erase the entity from the query's tracking list if the component was removed from the entity.
                    lastModified = m_clock.elapsedTime();
                }
            }
            else if (m_registry.getEntityData(entityId).components.contains(m_componentTypes[queryId]))
            {
                entityList.insert(entity); // If the entity also contains all the other required components for this query, then add this entity to the tracking list.
                lastModified = m_clock.elapsedTime();
            }
        }
    }

    void QueryRegistry::markEntityDestruction(id_type entityId)
    {
        OPTICK_EVENT();
        entity_handle entity(entityId);

        async::readwrite_guard guard(m_entityLock);
        for (int i = 0; i < m_entityLists.size(); i++) // Iterate over all query tracking lists.
        {
            id_type queryId = m_entityLists.keys()[i];
            auto& [lastModified, entityList] = m_entityLists.at(queryId);
            if (entityList.contains(entity))
            {
                entityList.erase(entity); // Erase entity from tracking list if it's present.
                lastModified = m_clock.elapsedTime();
            }
        }
    }

    id_type QueryRegistry::getQueryId(const hashed_sparse_set<id_type>& componentTypes)
    {
        OPTICK_EVENT();
        async::readonly_guard guard(m_componentLock);

        for (auto [id, types] : m_componentTypes)
        {
            if (types == componentTypes) // Iterate over all component type lists of all queries and check if it's the same as the requested list.
                return id;
        }

        return invalid_id;
    }

    EntityQuery QueryRegistry::createQuery(const hashed_sparse_set<id_type>& componentTypes)
    {
        OPTICK_EVENT();
        id_type queryId = getQueryId(componentTypes); // Check if a query already exists with the requested component types. 

        if (!queryId)
        {
            queryId = addQuery(componentTypes); // Create a new query if one doesn't exist yet.
        }

        return EntityQuery(queryId, this, &m_registry);
    }

    const hashed_sparse_set<id_type>& QueryRegistry::getComponentTypes(id_type queryId)
    {
        OPTICK_EVENT();
        async::readonly_guard guard(m_componentLock);
        return m_componentTypes[queryId];
    }

    id_type QueryRegistry::addQuery(const hashed_sparse_set<id_type>& componentTypes)
    {
        OPTICK_EVENT();
        id_type queryId;

        { // Write permitted critical section for m_entityLists
            async::readwrite_multiguard mguard(m_referenceLock, m_entityLock, m_componentLock);

            queryId = m_lastQueryId++;
            m_entityLists.emplace(queryId); // Create a new entity tracking list.

            m_references.emplace(queryId); // Create a new reference count.

            m_componentTypes.emplace(queryId, componentTypes); // Insert component type list for query.
        }

        { // Next we need to filter through all the entities to get all the new ones that apply to the new query.
            auto [entities, entitiesLock] = m_registry.getEntities(); // getEntities returns a pair of both the container as well as the lock that should be locked by you when operating on it.
            async::mixed_multiguard mguard(entitiesLock, async::lock_state_read, m_entityLock, async::lock_state_write); // Lock locks.
            auto& [lastModified, entityList] = m_entityLists.at(queryId);

            for (entity_handle entity : entities) // Iterate over all entities.
                if (m_registry.getEntityData(entity).components.contains(componentTypes)) // Check if the queried components completely overlaps the components in the entity.
                {
                    entityList.insert(entity); // Insert entity into tracking list.
                }

            lastModified = m_clock.elapsedTime();
        }

        return queryId;
    }

    const entity_container& QueryRegistry::getEntities(id_type queryId)
    {
        OPTICK_EVENT();
        async::readonly_multiguard entguard(m_entityLock, m_componentLock);
        auto& [lastModified, entityList] = m_entityLists.at(queryId);
        auto& [localModified, localList] = m_localCopies[queryId];
        if (lastModified > localModified)
        {
            {
                OPTICK_EVENT("Get entities");
                localList.clear();
                localList.assign(entityList.begin(), entityList.end());
            }

            auto& localComps = m_localComponents[queryId];
            auto& compTypes = m_componentTypes.at(queryId);

            for (auto compType : compTypes)
            {
                if (!localComps.count(compType))
                    localComps.emplace(compType, std::unique_ptr<component_container_base>(m_registry.getFamily(compType)->get_components(localList)));
                else
                    m_registry.getFamily(compType)->get_components(localList, *localComps.at(compType));
            }

            {
                OPTICK_EVENT("Remove old component types");
                std::vector<id_type> toRemove;

                for (auto& [compType, compList] : localComps)
                {
                    if (!compTypes.contains(compType))
                        toRemove.push_back(compType);
                }

                for (auto compType : toRemove)
                    localComps.erase(compType);
            }
        }

        return localList;
    }

    component_container_base& QueryRegistry::getComponents(id_type queryId, id_type componentTypeId)
    {
        return *m_localComponents.at(queryId).at(componentTypeId);
    }

    void QueryRegistry::submit(id_type queryId, id_type componentTypeId)
    {
        m_registry.getFamily(componentTypeId)->set_components(m_localCopies.at(queryId).second, *(m_localComponents.at(queryId).at(componentTypeId)));
    }

    void QueryRegistry::addReference(id_type queryId)
    {
        OPTICK_EVENT();
        async::readonly_guard refguard(m_referenceLock);

        m_references.at(queryId)++;
    }

    void QueryRegistry::removeReference(id_type queryId)
    {
        OPTICK_EVENT();
        if (queryId == invalid_id)
            return;

        async::readonly_guard refguard(m_referenceLock);

        if (!m_references.contains(queryId))
            return;

        size_type& referenceCount = m_references.at(queryId);
        referenceCount--;

        if (referenceCount == 0) // If there are no more references to this query then erase the query to reduce memory footprint.
        {
            async::readwrite_multiguard mguard(m_referenceLock, m_entityLock, m_componentLock);

            m_references.erase(queryId);
            m_entityLists.erase(queryId);
            m_componentTypes.erase(queryId);
        }
    }

    size_type QueryRegistry::getReferenceCount(id_type queryId)
    {
        OPTICK_EVENT();
        async::readonly_guard refguard(m_referenceLock);
        if (m_references.contains(queryId))
            return m_references.at(queryId);
        return 0;
    }
}
