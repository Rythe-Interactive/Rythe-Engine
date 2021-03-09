#include <core/ecs/registry.hpp>
#include <core/ecs/filters/filterregistry.hpp>

namespace legion::core::ecs
{
    std::unordered_map<id_type, std::unique_ptr<component_pool_base>> Registry::m_componentFamilies;
    std::unordered_map<id_type, std::unordered_set<id_type>> Registry::m_entityComposition;
    std::unordered_map<id_type, entity_data> Registry::m_entities;
    std::queue<id_type> Registry::m_recyclableEntities;

    L_NODISCARD entity Registry::getWorld()
    {
        auto& [_, data] = *m_entities.try_emplace(world_entity_id).first;
        data.alive = true;
        data.active = true;
        data.id = world_entity_id;
        data.parent = entity{ nullptr };

        m_entityComposition.try_emplace(entity{ &data });
        return entity{ &data };
    }

    entity world = Registry::getWorld();

    L_NODISCARD component_pool_base* Registry::getFamily(id_type typeId)
    {
        return m_componentFamilies.at(typeId).get();
    }

    entity Registry::createEntity()
    {
        return createEntity(world);
    }

    entity Registry::createEntity(entity parent)
    {
        static id_type nextEntityId = 1;
        const id_type currentEntityId =
            m_recyclableEntities.empty() ?
            ++nextEntityId :
            []()
        {
            id_type temp = m_recyclableEntities.front();
            m_recyclableEntities.pop();
            return temp;
        }();

        auto& [_, data] = *m_entities.try_emplace(currentEntityId).first;
        data.alive = true;
        data.active = true;
        data.id = currentEntityId;
        data.parent = parent;

        if (parent)
            parent->children.insert(entity{ &data });

        m_entityComposition.try_emplace(entity{ &data });
        return entity{ &data };
    }

    entity Registry::createEntity(entity parent, const serialization::entity_prototype& prototype)
    {
        const auto ent = createEntity(parent);
        for (auto& childPrototype : prototype.children)
            createEntity(ent, childPrototype);

        for (auto& [type, prototypePtr] : prototype.composition)
            getFamily(type)->create_component(ent, *prototypePtr);

        FilterRegistry::markEntityFullCreation(ent);

        return ent;
    }

    entity Registry::createEntity(const serialization::entity_prototype& prototype)
    {
        return createEntity(world, prototype);
    }

    void Registry::destroyEntity(entity target, bool recurse)
    {
        FilterRegistry::markEntityDestruction(target);

        auto& composition = m_entityComposition.at(target);

        for (auto& child : target->children)
            if (recurse)
                destroyEntity(child, recurse);
            else
                child.set_parent(world);

        if (target->parent)
        {
            target->parent->children.erase(target);
        }

        target->parent = entity{ nullptr };
        target->alive = false;
        target->active = false;
        target->children.clear();

        for (auto& componentId : composition)
            getFamily(componentId)->destroy_component(target);
        composition.clear();

        m_recyclableEntities.push(target->id);
        target->id = invalid_id;
    }

    void Registry::destroyEntity(id_type target, bool recurse)
    {
        destroyEntity(entity{ &m_entities.at(target) }, recurse);
    }

    bool Registry::checkEntity(entity target)
    {
        return target != nullptr;
    }

    bool Registry::checkEntity(id_type target)
    {
        return m_entities.count(target) && m_entities.at(target).alive;
    }

    L_NODISCARD std::unordered_map<id_type, std::unordered_set<id_type>>& Registry::entityCompositions()
    {
        return m_entityComposition;
    }

    L_NODISCARD std::unordered_map<id_type, entity_data>& Registry::entityData()
    {
        return m_entities;
    }

    L_NODISCARD std::unordered_set<id_type>& Registry::entityComposition(entity target)
    {
        return m_entityComposition.at(target);
    }

    L_NODISCARD std::unordered_set<id_type>& Registry::entityComposition(id_type target)
    {
        return m_entityComposition.at(entity{ &m_entities.at(target) });
    }

    L_NODISCARD entity_data& Registry::entityData(id_type target)
    {
        return m_entities.at(target);
    }

    void* Registry::createComponent(id_type typeId, entity target)
    {
        m_entityComposition.at(target).insert(typeId);
        FilterRegistry::markComponentAdd(typeId, target);
        return getFamily(typeId)->create_component(target);
    }

    void* Registry::createComponent(id_type typeId, entity target, const serialization::component_prototype_base& prototype)
    {
        m_entityComposition.at(target).insert(typeId);
        FilterRegistry::markComponentAdd(typeId, target);
        return getFamily(typeId)->create_component(target, prototype);
    }

    void* Registry::createComponent(id_type typeId, entity target, serialization::component_prototype_base&& prototype)
    {
        m_entityComposition.at(target).insert(typeId);
        FilterRegistry::markComponentAdd(typeId, target);
        return getFamily(typeId)->create_component(target, std::move(prototype));
    }

    void Registry::destroyComponent(id_type typeId, entity target)
    {
        m_entityComposition.at(target).erase(typeId);
        FilterRegistry::markComponentErase(typeId, target);
        getFamily(typeId)->destroy_component(target);
    }

    bool Registry::hasComponent(id_type typeId, entity target)
    {
        return getFamily(typeId)->contains(target);
    }

    void* Registry::getComponent(id_type typeId, entity target)
    {
        return getFamily(typeId)->get_component(target);
    }

}
