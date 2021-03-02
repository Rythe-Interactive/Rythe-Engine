#include <core/ecs/handles/entity.hpp>

#include <core/ecs/registry.hpp>

namespace legion::core::ecs
{
    entity::operator const id_type& () const noexcept
    {
        return id;
    }

    entity::operator id_type& () noexcept
    {
        return id;
    }

    void entity::set_parent(id_type parent) const
    {
        auto& hierarchy = Registry::entityHierarchy(id);
        if (hierarchy.parent)
            Registry::entityHierarchy(hierarchy.parent).children.erase(*this);

        if (parent)
            Registry::entityHierarchy(parent).children.insert(*this);

        hierarchy.parent = entity{ parent };
    }

    void entity::set_parent(entity parent) const
    {
        auto& hierarchy = Registry::entityHierarchy(id);
        if (hierarchy.parent)
            Registry::entityHierarchy(hierarchy.parent).children.erase(*this);

        if (parent)
            Registry::entityHierarchy(parent).children.insert(*this);

        hierarchy.parent = parent;
    }

    void entity::add_child(id_type child) const
    {
        add_child(entity{ child });
    }

    void entity::add_child(entity child) const
    {
        child.set_parent(id);
    }

    void entity::remove_child(id_type child) const
    {
        remove_child(entity{ child });
    }

    void entity::remove_child(entity child) const
    {
        child.set_parent(world_entity_id);
    }

}
