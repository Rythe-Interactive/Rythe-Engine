#include <core/ecs/entity_handle.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_handle.hpp>

namespace args::core::ecs
{
    struct child_iterator::impl
    {
        entity_set::iterator iterator;

        impl(entity_set::iterator it) : iterator(it) {}
    };

    child_iterator::child_iterator(impl* implptr) : m_pimpl(implptr) {}

    entity_handle& entity_handle::operator=(const entity_handle& other)
    {
        m_id = other.m_id;
        m_registry = other.m_registry;
        return *this;
    }

    A_NODISCARD   const hashed_sparse_set<id_type>& entity_handle::component_composition() const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        return m_registry->getEntityData(m_id).components;
    }

    A_NODISCARD id_type entity_handle::get_id() const
    {
        if (valid())
            return m_id;
        return invalid_id;
    }

    A_NODISCARD child_iterator entity_handle::begin() const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        return child_iterator(new child_iterator::impl(m_registry->getEntityData(m_id).children.begin()));
    }

    A_NODISCARD child_iterator entity_handle::end() const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        return child_iterator(new child_iterator::impl(m_registry->getEntityData(m_id).children.end()));
    }

    A_NODISCARD entity_handle entity_handle::get_parent() const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        return m_registry->getEntity(m_registry->getEntityData(m_id).parent);
    }

    void entity_handle::set_parent(id_type newParent) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        entity_data& data = m_registry->getEntityData(m_id);

        if (m_registry->validateEntity(data.parent))
            m_registry->getEntityData(data.parent).children.erase(*this);

        if (m_registry->validateEntity(newParent))
        {
            data.parent = newParent;

            m_registry->getEntityData(data.parent).children.insert(*this);
        }
        else
            data.parent = invalid_id;
    }

    A_NODISCARD   entity_handle entity_handle::operator[](index_type index) const
    {
        return get_child(index);
    }

    A_NODISCARD   entity_handle entity_handle::get_child(index_type index) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;

        entity_set& children = m_registry->getEntityData(m_id).children;

        return children[index];
    }

    A_NODISCARD size_type entity_handle::child_count() const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        return m_registry->getEntityData(m_id).children.size();
    }

    void entity_handle::add_child(id_type childId) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        entity_data& data = m_registry->getEntityData(m_id);

        entity_handle child = m_registry->getEntity(childId);

        if (child && !data.children.contains(child))
            child.set_parent(m_id);
    }

    void entity_handle::remove_child(id_type childId) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        entity_data& data = m_registry->getEntityData(m_id);
        entity_handle child = m_registry->getEntity(childId);

        if (child && data.children.contains(child))
            child.set_parent(invalid_id);
    }

    A_NODISCARD   bool entity_handle::has_component(id_type componentTypeId) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        return m_registry->getEntityData(m_id).components.contains(componentTypeId);
    }

    A_NODISCARD   component_handle_base entity_handle::get_component_handle(id_type componentTypeId) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        return m_registry->getComponent(m_id, componentTypeId);
    }

    component_handle_base entity_handle::add_component(id_type componentTypeId) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        return m_registry->createComponent(m_id, componentTypeId);
    }

    component_handle_base entity_handle::add_component(id_type componentTypeId, void* value) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        return m_registry->createComponent(m_id, componentTypeId, value);
    }

    void entity_handle::remove_component(id_type componentTypeId) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        m_registry->destroyComponent(m_id, componentTypeId);
    }

    void entity_handle::destroy(bool recurse) const
    {
        if (!m_registry)
            throw args_invalid_entity_error;
        m_registry->destroyEntity(m_id);
    }

    bool entity_handle::valid() const
    {
        if (m_registry && m_id)
            if (m_registry->validateEntity(m_id))
                return true;

        return false;
    }

    bool operator==(const child_iterator& lhs, const child_iterator& rhs)
    {
        return lhs.m_pimpl->iterator->get_id() == rhs.m_pimpl->iterator->get_id();
    }

    entity_handle& child_iterator::operator*()
    {
        return *m_pimpl->iterator;
    }

    entity_handle* child_iterator::operator->()
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
}
