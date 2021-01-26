#include <core/ecs/entity_handle.hpp>
#include <core/ecs/ecsregistry.hpp>
#include <core/ecs/component_handle.hpp>
#include <core/defaults/defaultcomponents.hpp>
#include <Optick/optick.h>

namespace legion::core::ecs
{
    EcsRegistry* entity_handle::m_registry = nullptr;
    events::EventBus* entity_handle::m_eventBus = nullptr;

    entity_handle& entity_handle::operator=(const entity_handle& other) noexcept
    {
        m_id = other.m_id;
        return *this;
    }

    entity_handle entity_handle::clone(bool keep_parent, bool clone_children, bool clone_components) const
    {
        OPTICK_EVENT();
        entity_handle clone = m_registry->createEntity();
        entity_data data = m_registry->getEntityData(get_id());

        if (has_component<hierarchy>())
        {
            hierarchy hry = read_component<hierarchy>();

            if (keep_parent)
                clone.set_parent(hry.parent, false);

            if (clone_children)
                for (const entity_handle& h : hry.children)
                {
                    h.clone(false, true, true).set_parent(clone, false);
                }
        }

        if (clone_components)
            for (id_type cid : data.components)
            {
                m_registry->copyComponent(clone, *this, cid);
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

    L_NODISCARD entity_set entity_handle::children() const
    {
        return read_component<hierarchy>().children;
    }

    L_NODISCARD entity_handle entity_handle::get_parent() const
    {
        OPTICK_EVENT();
        return m_registry->getEntityParent(m_id);
    }

    void entity_handle::set_parent(id_type newParent, bool addHierarchyIfAbsent)
    {
        bool hasHierarchy = true;
        if (!has_component<hierarchy>())
        {
            if (addHierarchyIfAbsent)
                add_component<hierarchy>();
            else
                hasHierarchy = false;
        }

        if (hasHierarchy)
        {
            OPTICK_EVENT();

            hierarchy data = read_component<hierarchy>();
            entity_handle previousParent = invalid_id;

#ifdef LGN_SAFE_MODE
            if (m_registry->validateEntity(data.parent))
#else
            if (data.parent.get_id())
#endif
            {
                previousParent = data.parent;
                auto parentData = data.parent.read_component<hierarchy>();
                parentData.children.erase(*this);
                data.parent.write_component<hierarchy>(parentData);
            }

            data.parent = newParent;

#ifdef LGN_SAFE_MODE
            if (m_registry->validateEntity(newParent))
#else
            if (newParent)
#endif
            {
                if (data.parent.has_component<hierarchy>())
                {
                    auto parentData = data.parent.read_component<hierarchy>();
                    parentData.children.insert(*this);
                    data.parent.write_component<hierarchy>(parentData);
                }
                else if (addHierarchyIfAbsent)
                {
                    hierarchy parentData;
                    parentData.children.insert(*this);
                    data.parent.add_component<hierarchy>(parentData);
                }
                else
                    data.parent = invalid_id;
            }
            else
                data.parent = invalid_id;

            write_component<hierarchy>(data);
            m_eventBus->raiseEvent<events::parent_change>(*this, previousParent, data.parent);
        }
        else
        {
            OPTICK_EVENT();

            entity_handle parent = newParent;

#ifdef LGN_SAFE_MODE
            if (m_registry->validateEntity(newParent))
#else
            if (newParent && parent.has_component<hierarchy>())
#endif
            {
                auto parentData = parent.read_component<hierarchy>();
                parentData.children.insert(*this);
                parent.write_component<hierarchy>(parentData);
            }
            else
                parent = invalid_id;

            m_eventBus->raiseEvent<events::parent_change>(*this, entity_handle(invalid_id), parent);
        }

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
        for (auto child : read_component<hierarchy>().children)
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
        for (auto child : read_component<hierarchy>().children)
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
        if (!has_component<hierarchy>())
            return entity_handle(invalid_id);

        OPTICK_EVENT();
        return read_component<hierarchy>().children[index];
    }

    L_NODISCARD size_type entity_handle::child_count() const
    {
        if (!has_component<hierarchy>())
            return 0;

        OPTICK_EVENT();
        return read_component<hierarchy>().children.size();
    }

    void entity_handle::add_child(id_type childId)
    {
        if (!has_component<hierarchy>())
            m_registry->getEntity(childId).add_component<hierarchy>();

        OPTICK_EVENT();
        entity_handle child = m_registry->getEntity(childId);

        if (child.m_id && !read_component<hierarchy>().children.contains(child))
            child.set_parent(m_id);
    }

    void entity_handle::remove_child(id_type childId)
    {
        if (!has_component<hierarchy>())
            return;

        OPTICK_EVENT();
        entity_handle child = m_registry->getEntity(childId);

        if (child.m_id && read_component<hierarchy>().children.contains(child))
            child.set_parent(world_entity_id, false);
    }

    L_NODISCARD bool entity_handle::has_component(id_type componentTypeId) const
    {
        OPTICK_EVENT();
        return m_registry->hasComponent(m_id, componentTypeId);
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
        m_id = invalid_id;
    }

    bool entity_handle::valid() const
    {
        OPTICK_EVENT();
        return m_registry->validateEntity(m_id);
    }
    }
