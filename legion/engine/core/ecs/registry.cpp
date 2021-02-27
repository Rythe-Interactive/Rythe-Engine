#include <core/ecs/registry.hpp>

namespace legion::core::ecs
{
    std::unordered_map<id_type, std::unique_ptr<component_pool_base>> Registry::m_componentFamilies;
    std::unordered_map<id_type, std::unordered_set<id_type>> Registry::m_entityComposition;
    std::unordered_map<id_type, entity_hierarchy> Registry::m_entityHierarchy;
    std::queue<id_type> Registry::m_recyclableEntities;

    L_NODISCARD component_pool_base* Registry::getFamily(id_type typeId)
    {
        return m_componentFamilies.at(typeId).get();
    }

    entity Registry::createEntity(entity parent)
    {
        static id_type nextEntityId = 1;
        const id_type currentEntityId =
            m_recyclableEntities.empty() ?
            nextEntityId++ :
            []()
        {
            id_type temp = m_recyclableEntities.front();
            m_recyclableEntities.pop();
            return temp;
        }();

        auto& [_0, hierarchy] = *m_entityHierarchy.try_emplace(currentEntityId).first;
        hierarchy.parent = { world_entity_id };
        hierarchy.children.clear();

        m_entityHierarchy.at(world_entity_id).children.insert({ currentEntityId });

        auto& [_1, composition] = *m_entityComposition.try_emplace(currentEntityId).first;
        composition.clear();

        return { currentEntityId };
    }

    entity Registry::createEntity(entity parent, const serialization::entity_prototype& prototype)
    {
        const auto ent = createEntity(parent);
        for (auto& childPrototype : prototype.children)
            createEntity(ent, childPrototype);

        for (auto& [type, prototypePtr] : prototype.composition)
            createComponent(type, ent, *prototypePtr);

        return ent;
    }

    entity Registry::createEntity(const serialization::entity_prototype& prototype)
    {
        return createEntity(world, prototype);
    }

    void Registry::destroyEntity(entity target, bool recurse)
    {
        target.set_parent(invalid_id);

        for (auto& child : m_entityHierarchy.at(target).children)
            if (recurse)
                destroyEntity(child, recurse);
            else
                child.set_parent(world);

        for (auto& componentId : m_entityComposition.at(target))
            destroyComponent(componentId, target);

        m_recyclableEntities.push(target);
    }

    void Registry::destroyEntity(id_type target, bool recurse)
    {
        destroyEntity(entity{ target }, recurse);
    }

    L_NODISCARD std::unordered_set<id_type>& Registry::entityComposition(entity target)
    {
        return m_entityComposition.at(target);
    }

    L_NODISCARD std::unordered_set<id_type>& Registry::entityComposition(id_type target)
    {
        return m_entityComposition.at(target);
    }

    L_NODISCARD entity_hierarchy& Registry::entityHierarchy(entity target)
    {
        return m_entityHierarchy.at(target);
    }

    L_NODISCARD entity_hierarchy& Registry::entityHierarchy(id_type target)
    {
        return m_entityHierarchy.at(target);
    }

    void* Registry::createComponent(id_type typeId, entity target)
    {
        return getFamily(typeId)->create_component(target);
    }

    void* Registry::createComponent(id_type typeId, entity target, const serialization::component_prototype_base& prototype)
    {
        return getFamily(typeId)->create_component(target, prototype);
    }

    void Registry::destroyComponent(id_type typeId, entity target)
    {
        getFamily(typeId)->destroy_component(target);
    }

    void* Registry::getComponent(id_type typeId, entity target)
    {
        return getFamily(typeId)->get_component(target);
    }

}
