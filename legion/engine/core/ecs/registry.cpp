#include <core/ecs/registry.hpp>
#include <core/ecs/filters/filterregistry.hpp>

namespace legion::core::ecs
{
    component_pool_base* Registry::tryEmplaceFamily(id_type id, std::unique_ptr<component_pool_base>&& ptr)
    {
        auto& families = getFamilies();
        if (families.count(id)) // Check and fetch in order to avoid a possibly unnecessary allocation and deletion.
            return families.at(id).get();

        // Allocate and emplace if no item was found.
        return families.emplace(
            id,
            std::move(ptr)
        ).first->second.get(); // std::pair<iterator, bool>.first --> iterator<std::pair<key, value>>->second --> std::unique_ptr.get() --> component_pool_base*
    }
    std::unordered_map<id_type, std::unique_ptr<component_type_base>>& Registry::componentTypes()
    {
        static std::unordered_map<id_type, std::unique_ptr<component_type_base>> m_componentTypes;
        return m_componentTypes;
    }

    id_type Registry::getNextEntityId()
    {
        // Get the current entity ID by recycling or generating a new one.
        return instance.m_recyclableEntities.empty() ? // If there are no recyclable entityIDs
            instance.m_nextEntityId++ :               // Then we increase the next entityID and use that.
            []()
        {   // Lanbda to handle recycling case.
            id_type temp = instance.m_recyclableEntities.front(); // Copy out first item.
            instance.m_recyclableEntities.pop();                  // Remove first item from queue.
            return temp;
        }();

        // The above code could be improved with a custom threadsafe queue that wouldn't have to use the lambda.
        // Making this function threadsafe using atomics and locks could be very benificial.
    }

    entity Registry::getWorld()
    {
        init();

        // Create entity data.
        auto& [_, data] = *instance.m_entities.try_emplace(world_entity_id).first;
        data.alive = true;
        data.active = true;
        data.id = world_entity_id;
        data.name = "World";
        data.parent = entity{ nullptr };

        // Create component composition.
        instance.m_entityCompositions.try_emplace(world_entity_id);
        return entity{ &data };
    }

    // Assign world entity.
    entity world;

    void Registry::onInit()
    {
        create();

        for (auto& [id, componentType] : componentTypes())
            tryEmplaceFamily(id, componentType->create_pool());

        world = getWorld();
        reportDependency<FilterRegistry>();
    }

    void Registry::onShutdown()
    {
        for (auto& [_, family] : getFamilies())
            family->clear();
    }

    component_pool_base* Registry::getFamily(id_type typeId)
    {
        return getFamilies().at(typeId).get();
    }

    const std::string& Registry::getFamilyName(id_type id)
    {
        auto& types = componentTypes();
        if (const auto itr = types.find(id); itr != types.end())
        {
            return itr->second->componentName;
        }

        static std::string unknownCompName = "unkown component";
        return unknownCompName;
    }

    std::unordered_map<id_type, std::unique_ptr<component_pool_base>>& Registry::getFamilies()
    {
        return instance.m_componentFamilies;
    }

    entity Registry::createEntity()
    {
        // Default parent of any entity is the world.
        return createEntity(world);
    }

    entity Registry::createEntity(const std::string& name)
    {
        return createEntity(name, world);
    }

    entity Registry::createEntity(entity parent)
    {
        OPTICK_EVENT();

        const id_type currentEntityId = getNextEntityId();

        // We use try_emplace in order to preserve the memory pooling that the children set might have.
        // This way recycled entities might not have to do as much reallocation when new children are added.
        auto& [_, data] = *instance.m_entities.try_emplace(currentEntityId).first;
        data.alive = true;
        data.active = true;
        data.id = currentEntityId;
        data.name = std::to_string(currentEntityId);
        data.parent = parent;

        if (parent) // If our parent is a valid entity we want to add the new entity as a child to the parent.
            parent->children.insert(entity{ &data });

        // We try to insert another set for entity composition, if one already exists then we can recycle it.
        instance.m_entityCompositions.try_emplace(entity{ &data });

        // We create an identical entity handle multiple times in this function but it's fine because it only holds a pointer.
        // By keeping these identical entity handles inline rvalues the compiler is allowed to optimize them away.
        return entity{ &data };
    }

    entity Registry::createEntity(const std::string& name, entity parent)
    {
        OPTICK_EVENT();

        const id_type currentEntityId = getNextEntityId();

        // We use try_emplace in order to preserve the memory pooling that the children set might have.
        // This way recycled entities might not have to do as much reallocation when new children are added.
        auto& [_, data] = *instance.m_entities.try_emplace(currentEntityId).first;
        data.alive = true;
        data.active = true;
        data.id = currentEntityId;
        data.name = name;
        data.parent = parent;

        if (parent) // If our parent is a valid entity we want to add the new entity as a child to the parent.
            parent->children.insert(entity{ &data });

        // We try to insert another set for entity composition, if one already exists then we can recycle it.
        instance.m_entityCompositions.try_emplace(entity{ &data });

        // We create an identical entity handle multiple times in this function but it's fine because it only holds a pointer.
        // By keeping these identical entity handles inline rvalues the compiler is allowed to optimize them away.
        return entity{ &data };
    }

    void Registry::destroyEntity(entity target, bool recurse)
    {
        OPTICK_EVENT();
        // Remove entity from filters to stop it from updating.
        FilterRegistry::markEntityDestruction(target);

        // If the entity had a valid parent then we need to orphan this entity.
        if (target->parent)
            target->parent->children.erase(target);
        target->parent = entity{ nullptr };

        // Set entity to be dead and inactive.
        // If we did this before orphaning then the erasure would have failed.
        // Dead entities never equate to any other entities (Similar behaviour to NaN).
        target->alive = false;
        target->active = false;

        // Handle all children.
        for (auto& child : target->children)
            if (recurse)
                destroyEntity(child, recurse); // Either recursively destroy them as well.
            else
            {
                world->children.insert(child); // Or orphan them to the world entity.
                child->parent = world;
            }

        // Due to alive having been set before no children removed themselves from the children list.
        // So we need to clear the children list ourselves.
        target->children.clear();

        // Destroy every component in the composition and clear the composition.
        auto& composition = instance.m_entityCompositions.at(target->id);
        for (auto& componentId : composition)
            getFamily(componentId)->destroy_component(target);
        composition.clear();

        // Mark entity as recyclable and invalidate ID.
        instance.m_recyclableEntities.push(target->id);
        target->id = invalid_id;
    }

    void Registry::destroyEntity(id_type target, bool recurse)
    {
        destroyEntity(entity{ &instance.m_entities.at(target) }, recurse);
    }

    bool Registry::checkEntity(entity target)
    {
        OPTICK_EVENT();
        return target.data && target.data->alive;
    }

    bool Registry::checkEntity(id_type target)
    {
        OPTICK_EVENT();
        return instance.m_entities.count(target) && instance.m_entities.at(target).alive;
    }

    std::unordered_map<id_type, std::unordered_set<id_type>>& Registry::entityCompositions()
    {
        return instance.m_entityCompositions;
    }

    std::unordered_map<id_type, entity_data>& Registry::entityData()
    {
        return instance.m_entities;
    }

    std::unordered_set<id_type>& Registry::entityComposition(entity target)
    {
        return instance.m_entityCompositions.at(target);
    }

    std::unordered_set<id_type>& Registry::entityComposition(id_type target)
    {
        return instance.m_entityCompositions.at(target);
    }

    entity_data& Registry::entityData(id_type target)
    {
        return instance.m_entities.at(target);
    }

    entity Registry::getEntity(id_type target)
    {
        return entity{ &entityData(target) };
    }

    void* Registry::createComponent(id_type typeId, entity target)
    {
        OPTICK_EVENT();
        // Update entity composition.
        instance.m_entityCompositions.at(target).insert(typeId);
        // Update filters.
        FilterRegistry::markComponentAdd(typeId, target);
        // Actually create and return the component.
        return getFamily(typeId)->create_component(target);
    }

    void Registry::destroyComponent(id_type typeId, entity target)
    {
        OPTICK_EVENT();
        // Update entity composition.
        instance.m_entityCompositions.at(target).erase(typeId);
        // Update filters.
        FilterRegistry::markComponentErase(typeId, target);
        // Actually destroy the component.
        getFamily(typeId)->destroy_component(target);
    }

    bool Registry::hasComponent(id_type typeId, entity target)
    {
        OPTICK_EVENT();
        return getFamily(typeId)->contains(target);
    }

    void* Registry::getComponent(id_type typeId, entity target)
    {
        OPTICK_EVENT();
        return getFamily(typeId)->get_component(target);
    }

}
