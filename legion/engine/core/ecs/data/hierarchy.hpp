#pragma once
#include <core/containers/hashed_sparse_set.hpp>

#include <core/ecs/handles/entity.hpp>

namespace legion::core::ecs
{
    struct entity_hierarchy
    {
        entity parent;
        hashed_sparse_set<entity> children;
    };
}
