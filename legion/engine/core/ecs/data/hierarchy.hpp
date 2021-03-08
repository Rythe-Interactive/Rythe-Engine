#pragma once
#include <core/containers/hashed_sparse_set.hpp>

#include <core/ecs/handles/entity.hpp>

namespace legion::core::ecs
{
    struct entity_data
    {
        // entity state bitfield.
        bool alive : 1; // bool only needs 1 bit.
        bool active : 1;

        id_type id = invalid_id;

        entity parent = { nullptr };
        entity_set children;
    };
}
