#include <core/ecs/entity_handle.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_handle.hpp>
#include <Optick/optick.h>

namespace legion::core::ecs
{
    EcsRegistry* entity_handle::m_registry = nullptr;

    struct child_iterator::impl
    {
        entity_set::iterator iterator;

        impl(entity_set::iterator it) : iterator(it) {}
    };

    child_iterator::child_iterator(impl* implptr) : m_pimpl(implptr) {}

    entity_handle& entity_handle::operator=(const entity_handle& other)
    {
        m_id = other.m_id;
        return *this;
    }

    entity_handle entity_handle::clone(bool keep_parent, bool clone_children, bool clone_components) const
    {
        OPTICK_EVENT();
        entity_handle clone = m_registry->createEntity();
        entity_data data = m_registry->getEntityData(get_id());

        if(keep_parent)
            clone.set_parent(data.parent);

        if(clone_components)
            for(id_type cid : data.components)
            {
                m_registry->copyComponent(clone,*this,cid);
            }

        if(clone_children)
            for(const entity_handle& h: data.children)
            {
                h.clone(false,true,true).set_parent(clone);
            }

        return clone;
    }

    L_NODISCARD hashed_sparse_set<id_type> entity_handle::component_composition() const
    {
        OPTICK_EVENT();
        return m_registry->getEntityData(m_id).components;
    }

    L_NODISCARD id_type entity_handle::get_id() const
    {
        return m_id;
    }

    L_NODISCARD child_iterator entity_handle::begin() const
    {
        return child_iterator(new child_iterator::impl(m_registry->getEntityData(m_id).children.begin()));
    }

    L_NODISCARD child_iterator entity_handle::end() const
    {
        return child_iterator(new child_iterator::impl(m_registry->getEntityData(m_id).children.end()));
    }

    L_NODISCARD entity_handle entity_handle::get_parent() const
    {
        OPTICK_EVENT();
        return m_registry->getEntityParent(m_id);
    }

    void entity_handle::set_parent(id_type newParent)
    {
        OPTICK_EVENT();
        entity_data data = m_registry->getEntityData(m_id);

        if (m_registry->validateEntity(data.parent))
        {
            auto parentData = m_registry->getEntityData(data.parent);
            parentData.children.erase(*this);
            m_registry->setEntityData(data.parent, parentData);
        }
        if (m_registry->validateEntity(newParent))
        {
            data.parent = newParent;

            auto parentData = m_registry->getEntityData(data.parent);
            parentData.children.insert(*this);
            m_registry->setEntityData(data.parent, parentData);
        }
        else
            data.parent = invalid_id;

        m_registry->setEntityData(m_id, data);
    }

    void entity_handle::serialize(cereal::JSONOutputArchive& oarchive)
    {
        OPTICK_EVENT();
        std::vector <ecs::component_handle_base> components;
        std::vector <ecs::entity_handle> children;
        auto composition = component_composition();
        for (int i = 0; i < composition.size(); i++)
        {
            components.push_back(m_registry->getComponent(m_id, composition[i]));
        }
        for (auto child : m_registry->getEntityData(m_id).children)
        {
            children.push_back(child);
        }
        oarchive(cereal::make_nvp("Id", m_id), cereal::make_nvp("Name", std::string("Entity")));
        oarchive(cereal::make_nvp("Components", components), cereal::make_nvp("Children", children));
    }

    void entity_handle::serialize(cereal::BinaryOutputArchive& oarchive)
    {
        OPTICK_EVENT();
        std::vector <ecs::component_handle_base> components;
        std::vector <ecs::entity_handle> children;
        for (int i = 0; i < m_registry->getEntity(m_id).component_composition().size(); i++)
        {
            components.push_back(m_registry->getComponent(m_id, m_registry->getEntity(m_id).component_composition()[i]));
        }
        for (auto child : m_registry->getEntityData(m_id).children)
        {
            children.push_back(child);
        }
        oarchive(cereal::make_nvp("ID", m_id), cereal::make_nvp("NAME", std::string("ENTITY")));
        oarchive(cereal::make_nvp("COMPONENTS", components), cereal::make_nvp("CHILDREN", children));
    }

    void entity_handle::serialize(cereal::JSONInputArchive& oarchive)
    {
        OPTICK_EVENT();
        std::vector <ecs::component_handle_base> components;
        std::vector <ecs::entity_handle> children;
        oarchive(cereal::make_nvp("Id", m_id), cereal::make_nvp("Name", std::string("Entity")));
        entity_handle ent;
        if (!m_registry->validateEntity(m_id))
            ent = m_registry->createEntity(m_id);
        else
            ent = m_registry->getEntity(m_id);

        oarchive(cereal::make_nvp("Components", components), cereal::make_nvp("Children", children));
        for (auto child : children)
        {
            child.set_parent(m_id);
        }
    }

    void entity_handle::serialize(cereal::BinaryInputArchive& oarchive)
    {
        OPTICK_EVENT();
        std::vector <ecs::component_handle_base> components;
        std::vector <ecs::entity_handle> children;
        oarchive(cereal::make_nvp("ID", m_id), cereal::make_nvp("NAME", std::string("ENTITY")));
        auto ent = m_registry->createEntity(m_id);
        (void)ent;
        oarchive(cereal::make_nvp("COMPONENTS", components), cereal::make_nvp("CHILDREN", children));
        for (auto child : children)
        {
            child.set_parent(m_id);
        }
    }

    L_NODISCARD entity_handle entity_handle::operator[](index_type index) const
    {
        return get_child(index);
    }

    L_NODISCARD entity_handle entity_handle::get_child(index_type index) const
    {
        OPTICK_EVENT();
        return m_registry->getEntityData(m_id).children[index];
    }

    L_NODISCARD size_type entity_handle::child_count() const
    {
        OPTICK_EVENT();
        return m_registry->getEntityData(m_id).children.size();
    }

    void entity_handle::add_child(id_type childId)
    {
        OPTICK_EVENT();
        entity_data data = m_registry->getEntityData(m_id);

        entity_handle child = m_registry->getEntity(childId);

        if (child && !data.children.contains(child))
            child.set_parent(m_id);
    }

    void entity_handle::remove_child(id_type childId)
    {
        OPTICK_EVENT();
        entity_data data = m_registry->getEntityData(m_id);
        entity_handle child = m_registry->getEntity(childId);

        if (child && data.children.contains(child))
            child.set_parent(world_entity_id);
    }

    L_NODISCARD bool entity_handle::has_component(id_type componentTypeId) const
    {
        OPTICK_EVENT();
        return m_registry->getEntityData(m_id).components.contains(componentTypeId);
    }

    L_NODISCARD component_handle_base entity_handle::get_component_handle(id_type componentTypeId)
    {
        OPTICK_EVENT();
        return m_registry->getComponent(m_id, componentTypeId);
    }

    L_NODISCARD const component_handle_base entity_handle::get_component_handle(id_type componentTypeId) const
    {
        OPTICK_EVENT();
        return m_registry->getComponent(m_id, componentTypeId);
    }

    component_handle_base entity_handle::add_component(id_type componentTypeId)
    {
        OPTICK_EVENT();
        return m_registry->createComponent(m_id, componentTypeId);
    }

    component_handle_base entity_handle::add_component(id_type componentTypeId, void* value)
    {
        OPTICK_EVENT();
        return m_registry->createComponent(m_id, componentTypeId, value);
    }

    void entity_handle::remove_component(id_type componentTypeId)
    {
        OPTICK_EVENT();
        m_registry->destroyComponent(m_id, componentTypeId);
    }

    void entity_handle::destroy(bool recurse)
    {
        OPTICK_EVENT();
        m_registry->destroyEntity(m_id);
    }

    bool entity_handle::valid() const
    {
        OPTICK_EVENT();
        return m_registry->validateEntity(m_id);
    }

    bool operator==(const child_iterator& lhs, const child_iterator& rhs)
    {
        return lhs.m_pimpl->iterator == rhs.m_pimpl->iterator;
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
