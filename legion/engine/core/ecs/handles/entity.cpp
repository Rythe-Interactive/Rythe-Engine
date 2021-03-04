#include <core/ecs/handles/entity.hpp>

#include <core/ecs/registry.hpp>

namespace legion::core::ecs
{
    bool entity::operator ==(std::nullptr_t) const
    {
        return !Registry::checkEntity(*this);
    }

    bool entity::operator!=(std::nullptr_t) const
    {
        return Registry::checkEntity(*this);
    }

    entity::operator const id_type& () const noexcept
    {
        return id;
    }

    entity::operator id_type& () noexcept
    {
        return id;
    }

    void entity::set_parent(id_type parent)
    {
        auto& hierarchy = Registry::entityHierarchy(id);
        if (hierarchy.parent)
            Registry::entityHierarchy(hierarchy.parent).children.erase(*this);

        if (parent)
            Registry::entityHierarchy(parent).children.insert(*this);

        hierarchy.parent = entity{ parent };
    }

    void entity::set_parent(entity parent)
    {
        auto& hierarchy = Registry::entityHierarchy(id);
        if (hierarchy.parent)
            Registry::entityHierarchy(hierarchy.parent).children.erase(*this);

        if (parent)
            Registry::entityHierarchy(parent).children.insert(*this);

        hierarchy.parent = parent;
    }

    entity entity::get_parent() const
    {
        return Registry::entityHierarchy(id).parent;
    }

    void entity::add_child(id_type child)
    {
        add_child(entity{ child });
    }

    void entity::add_child(entity child)
    {
        child.set_parent(id);
    }

    void entity::remove_child(id_type child)
    {
        remove_child(entity{ child });
    }

    void entity::remove_child(entity child)
    {
        child.set_parent(world_entity_id);
    }

    entity_set& entity::children()
    {
        return Registry::entityHierarchy(id).children;
    }

    const entity_set& entity::children() const
    {
        return Registry::entityHierarchy(id).children;
    }

    entity_set::iterator entity::begin()
    {
        return Registry::entityHierarchy(id).children.begin();
    }

    entity_set::const_iterator entity::begin() const
    {
        return Registry::entityHierarchy(id).children.cbegin();
    }

    entity_set::const_iterator entity::cbegin() const
    {
        return Registry::entityHierarchy(id).children.cbegin();
    }

    entity_set::iterator entity::end()
    {
        return Registry::entityHierarchy(id).children.end();
    }

    entity_set::const_iterator entity::end() const
    {
        return Registry::entityHierarchy(id).children.cend();
    }

    entity_set::const_iterator entity::cend() const
    {
        return Registry::entityHierarchy(id).children.cend();
    }

    void entity::destroy(bool recurse)
    {
        Registry::destroyEntity(id, recurse);
    }

}
