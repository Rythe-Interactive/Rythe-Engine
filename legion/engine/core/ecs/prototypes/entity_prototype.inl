#include <core/ecs/prototypes/entity_prototype.hpp>
#pragma once

namespace legion::core::serialization
{
    inline prototype<ecs::entity>::prototype(const prototype& src) : children(src.children)
    {
        for (auto& [typeId, compPrototype] : src.composition)
            composition.emplace(typeId, compPrototype->copy());
    }

    inline prototype<ecs::entity>& prototype<ecs::entity>::operator=(const prototype& src)
    {
        children = src.children;

        for (auto& [typeId, compPrototype] : src.composition)
            composition.emplace(typeId, compPrototype->copy());

        return *this;
    }
}
