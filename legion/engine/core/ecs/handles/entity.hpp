#pragma once
#include <core/types/types.hpp>

namespace legion::core::ecs
{
    static constexpr id_type world_entity_id = 1;

    struct entity
    {
    public:
        id_type id;

        operator const id_type& () const;
        operator id_type& ();

        void set_parent(id_type id) const;
        void set_parent(entity child) const;

        void add_child(id_type id) const;
        void add_child(entity child) const;

        void remove_child(id_type id) const;
        void remove_child(entity child) const;
    };
}
