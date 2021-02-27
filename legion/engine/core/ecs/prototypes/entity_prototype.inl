#include <core/ecs/prototypes/entity_prototype.hpp>
#pragma once

namespace legion::core::serialization
{
    inline prototype<ecs::entity>::prototype(const prototype& src) : children(src.children)
    {
        for (auto& [typeId, compPrototype] : src.composition)
            composition.emplace(typeId, compPrototype->copy());
    }

    inline entity_prototype& prototype<ecs::entity>::operator=(const entity_prototype& src)
    {
        children = src.children;

        for (auto& [typeId, compPrototype] : src.composition)
            composition.emplace(typeId, compPrototype->copy());

        return *this;
    }
}
