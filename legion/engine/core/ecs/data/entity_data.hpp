#pragma once
#include <core/containers/hashed_sparse_set.hpp>

#include <core/ecs/handles/entity.hpp>

/**
 * @file entity_data.hpp
 */

namespace legion::core::ecs
{
    /**@class entity_data
     * @brief Entity specific data, eg: alive, active, children.
     */
    struct entity_data
    {
        // entity state bitfield.
        bool alive : 1; // bool only needs 1 bit.
        bool active : 1;

        id_type id = invalid_id;

        std::string name;
        entity parent = { nullptr };
        entity_set children;
    };
}
