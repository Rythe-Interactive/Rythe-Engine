#pragma once
#include <core/containers/hashed_sparse_set.hpp>

#include <core/ecs/handles/entity.hpp>

namespace legion::core::ecs
{
    struct entity_hierarchy
    {
        entity parent = { invalid_id };
        entity_set children;
    };

    struct entity_data
    {
        // entity state bitfield.
        bool alive : 1; // bool only needs 1 bit.
        bool active : 1;

        entity parent = { invalid_id };
        entity_set children;
    };
}
