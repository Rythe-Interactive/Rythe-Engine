#include <core/ecs/handles/entity.hpp>

#include <core/ecs/registry.hpp>

namespace legion::core::ecs
{
    struct child_iterator::impl
    {
        entity_set::iterator iterator;

        impl(entity_set::iterator it) : iterator(it) {}
    };

    child_iterator::child_iterator(impl* implptr) : m_pimpl(implptr) {}

    struct const_child_iterator::impl
    {
        entity_set::const_iterator iterator;

        impl(entity_set::const_iterator it) : iterator(it) {}
    };

    const_child_iterator::const_child_iterator(impl* implptr) : m_pimpl(implptr) {}

    struct child_reverse_iterator::impl
    {
        entity_set::reverse_iterator iterator;

        impl(entity_set::reverse_iterator it) : iterator(it) {}
    };

    child_reverse_iterator::child_reverse_iterator(impl* implptr) : m_pimpl(implptr) {}

    struct const_child_reverse_iterator::impl
    {
        entity_set::const_reverse_iterator iterator;

        impl(entity_set::const_reverse_iterator it) : iterator(it) {}
    };

    const_child_reverse_iterator::const_child_reverse_iterator(impl* implptr) : m_pimpl(implptr) {}

    entity::operator id_type () const noexcept
    {
        return (data && data->alive) ? data->id : invalid_id;
    }

    bool entity::valid() const noexcept
    {
        return data && data->alive;
    }

    entity_data* entity::operator->() noexcept
    {
        return data;
    }

    const entity_data* entity::operator->() const noexcept
    {
        return data;
    }

    std::unordered_set<id_type>& entity::component_composition()
    {
        return Registry::entityComposition(data->id);
    }

    const std::unordered_set<id_type>& entity::component_composition() const
    {
        return Registry::entityComposition(data->id);
    }

    void entity::set_parent(id_type parent)
    {
        if (data->parent)
            data->parent->children.erase(*this);

        if (parent)
        {
            Registry::entityData(parent).children.insert(*this);

            data->parent = entity{ &Registry::entityData(parent) };
        }
        else
            data->parent = entity{ nullptr };
    }

    void entity::set_parent(entity parent)
    {
        if (data->parent)
            data->parent->children.erase(*this);

        if (parent)
            parent->children.insert(*this);

        data->parent = parent;
    }

    entity entity::get_parent() const
    {
        return data->parent;
    }

    void entity::add_child(id_type child)
    {
        add_child(entity{ &Registry::entityData(child) });
    }

    void entity::add_child(entity child)
    {
        child.set_parent(*this);
    }

    void entity::remove_child(id_type child)
    {
        remove_child(entity{ &Registry::entityData(child) });
    }

    void entity::remove_child(entity child)
    {
        child.set_parent(world);
    }

    void entity::remove_children()
    {
        for (auto& child : data->children)
        {
            world->children.insert(child);
            child->parent = world;
        }

        data->children.clear();
    }

    void entity::destroy_children(bool recurse)
    {
        for (auto& child : data->children.reverse_range())
            Registry::destroyEntity(child, recurse);
    }

    entity_set& entity::children()
    {
        return data->children;
    }

    const entity_set& entity::children() const
    {
        return data->children;
    }

    entity entity::get_child(size_type index) const
    {
        return data->children.at(index);
    }

    child_iterator entity::begin()
    {
        return child_iterator(new child_iterator::impl(data->children.begin()));
    }

    const_child_iterator entity::begin() const
    {
        return const_child_iterator(new const_child_iterator::impl(data->children.cbegin()));
    }

    const_child_iterator entity::cbegin() const
    {
        return const_child_iterator(new const_child_iterator::impl(data->children.cbegin()));
    }

    child_reverse_iterator entity::rbegin()
    {
        return child_reverse_iterator(new child_reverse_iterator::impl(data->children.rbegin()));
    }

    const_child_reverse_iterator entity::rbegin() const
    {
        return const_child_reverse_iterator(new const_child_reverse_iterator::impl(data->children.crbegin()));
    }

    const_child_reverse_iterator entity::crbegin() const
    {
        return const_child_reverse_iterator(new const_child_reverse_iterator::impl(data->children.crbegin()));
    }

    child_iterator entity::end()
    {
        return child_iterator(new child_iterator::impl(data->children.end()));
    }

    const_child_iterator entity::end() const
    {
        return const_child_iterator(new const_child_iterator::impl(data->children.cend()));
    }

    const_child_iterator entity::cend() const
    {
        return const_child_iterator(new const_child_iterator::impl(data->children.cend()));
    }

    child_reverse_iterator entity::rend()
    {
        return child_reverse_iterator(new child_reverse_iterator::impl(data->children.rend()));
    }

    const_child_reverse_iterator entity::rend() const
    {
        return const_child_reverse_iterator(new const_child_reverse_iterator::impl(data->children.crend()));
    }

    const_child_reverse_iterator entity::crend() const
    {
        return const_child_reverse_iterator(new const_child_reverse_iterator::impl(data->children.crend()));
    }

    void entity::destroy(bool recurse)
    {
        Registry::destroyEntity(*this, recurse);
    }

    bool operator==(const child_iterator& lhs, const child_iterator& rhs)
    {
        return lhs.m_pimpl->iterator == rhs.m_pimpl->iterator;
    }

    entity& child_iterator::operator*()
    {
        return *m_pimpl->iterator;
    }

    entity* child_iterator::operator->()
    {
        return &*m_pimpl->iterator;
    }

    child_iterator& child_iterator::operator++()
    {
        ++m_pimpl->iterator;
        return *this;
    }

    child_iterator& child_iterator::operator--()
    {
        --m_pimpl->iterator;
        return *this;
    }

    child_iterator child_iterator::operator++(int)
    {
        impl* prev = new impl(m_pimpl->iterator);

        m_pimpl->iterator++;

        return child_iterator(prev);
    }

    child_iterator child_iterator::operator--(int)
    {
        impl* prev = new impl(m_pimpl->iterator);

        m_pimpl->iterator--;

        return child_iterator(prev);
    }

    bool operator==(const const_child_iterator& lhs, const const_child_iterator& rhs)
    {
        return lhs.m_pimpl->iterator == rhs.m_pimpl->iterator;
    }

    const entity& const_child_iterator::operator*()
    {
        return *m_pimpl->iterator;
    }

    const entity* const_child_iterator::operator->()
    {
        return &*m_pimpl->iterator;
    }

    const_child_iterator& const_child_iterator::operator++()
    {
        ++m_pimpl->iterator;
        return *this;
    }

    const_child_iterator& const_child_iterator::operator--()
    {
        --m_pimpl->iterator;
        return *this;
    }

    const_child_iterator const_child_iterator::operator++(int)
    {
        impl* prev = new impl(m_pimpl->iterator);

        m_pimpl->iterator++;

        return const_child_iterator(prev);
    }

    const_child_iterator const_child_iterator::operator--(int)
    {
        impl* prev = new impl(m_pimpl->iterator);

        m_pimpl->iterator--;

        return const_child_iterator(prev);
    }

    bool operator==(const child_reverse_iterator& lhs, const child_reverse_iterator& rhs)
    {
        return lhs.m_pimpl->iterator == rhs.m_pimpl->iterator;
    }

    entity& child_reverse_iterator::operator*()
    {
        return *m_pimpl->iterator;
    }

    entity* child_reverse_iterator::operator->()
    {
        return &*m_pimpl->iterator;
    }

    child_reverse_iterator& child_reverse_iterator::operator++()
    {
        ++m_pimpl->iterator;
        return *this;
    }

    child_reverse_iterator& child_reverse_iterator::operator--()
    {
        --m_pimpl->iterator;
        return *this;
    }

    child_reverse_iterator child_reverse_iterator::operator++(int)
    {
        impl* prev = new impl(m_pimpl->iterator);

        m_pimpl->iterator++;

        return child_reverse_iterator(prev);
    }

    child_reverse_iterator child_reverse_iterator::operator--(int)
    {
        impl* prev = new impl(m_pimpl->iterator);

        m_pimpl->iterator--;

        return child_reverse_iterator(prev);
    }

    bool operator==(const const_child_reverse_iterator& lhs, const const_child_reverse_iterator& rhs)
    {
        return lhs.m_pimpl->iterator == rhs.m_pimpl->iterator;
    }

    const entity& const_child_reverse_iterator::operator*()
    {
        return *m_pimpl->iterator;
    }

    const entity* const_child_reverse_iterator::operator->()
    {
        return &*m_pimpl->iterator;
    }

    const_child_reverse_iterator& const_child_reverse_iterator::operator++()
    {
        ++m_pimpl->iterator;
        return *this;
    }

    const_child_reverse_iterator& const_child_reverse_iterator::operator--()
    {
        --m_pimpl->iterator;
        return *this;
    }

    const_child_reverse_iterator const_child_reverse_iterator::operator++(int)
    {
        impl* prev = new impl(m_pimpl->iterator);

        m_pimpl->iterator++;

        return const_child_reverse_iterator(prev);
    }

    const_child_reverse_iterator const_child_reverse_iterator::operator--(int)
    {
        impl* prev = new impl(m_pimpl->iterator);

        m_pimpl->iterator--;

        return const_child_reverse_iterator(prev);
    }
}
