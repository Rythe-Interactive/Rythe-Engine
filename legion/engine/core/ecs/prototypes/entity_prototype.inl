#include <core/ecs/prototypes/entity_prototype.hpp>
#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::serialization
{
    inline prototype<ecs::entity>::prototype(ecs::entity src) : active(src->active)
    {
        children.reserve(src->children.size());
        for (size_type i = 0; i < src->children.size(); i++)
        {
            children.emplace_back(src->children[i]);
        }

        for (auto typeId : ecs::Registry::entityComposition(src))
            composition.emplace(typeId, ecs::Registry::getFamily(typeId)->create_prototype(src));
    }

    inline prototype<ecs::entity>::prototype(const prototype& src) : active(src.active), children(src.children)
    {
        for (auto& [typeId, compPrototype] : src.composition)
            composition.emplace(typeId, compPrototype->copy());
    }

    inline prototype<ecs::entity>& prototype<ecs::entity>::operator=(ecs::entity src)
    {
        active = src->active;
        children.clear();
        children.reserve(src->children.size());
        for (size_type i = 0; i < src->children.size(); i++)
        {
            children.emplace_back(src->children[i]);
        }

        composition.clear();
        for (auto typeId : ecs::Registry::entityComposition(src))
            composition.emplace(typeId, ecs::Registry::getFamily(typeId)->create_prototype(src));

        return *this;
    }

    inline prototype<ecs::entity>& prototype<ecs::entity>::operator=(const prototype& src)
    {
        active = src.active;
        children = src.children;

        for (auto& [typeId, compPrototype] : src.composition)
            composition.emplace(typeId, compPrototype->copy());

        return *this;
    }
}
