#pragma once
#include <core/events/event.hpp>
#include <core/ecs/entity_handle.hpp>

namespace legion::core::events
{
    struct exit final : public event<exit>
    {
        const int exitcode;

        exit(int exitcode = 0);

        virtual bool persistent() override { return true; }

    };

    struct parent_change : public event<parent_change>
    {
        ecs::entity_handle oldParent;
        ecs::entity_handle newParent;
        ecs::entity_handle entity;
        parent_change() = default;
        parent_change(ecs::entity_handle entity, id_type oldParent, id_type newParent) : oldParent(oldParent), newParent(newParent), entity(entity) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }

    };

    template<typename component_type>
    struct component_creation : public event<component_creation<component_type>>
    {
        ecs::entity_handle entity;

        component_creation() = default;
        component_creation(ecs::entity_handle entity) : entity(entity) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }

    };

    template<typename component_type>
    struct component_modification : public event<component_modification<component_type>>
    {
        component_type oldValue;
        component_type newValue;
        ecs::entity_handle entity;

        component_modification() = default;
        component_modification(ecs::entity_handle entity, component_type&& oldVal, const component_type& newVal) : oldValue(oldVal), newValue(newVal), entity(entity) {}
        component_modification(ecs::entity_handle entity, const component_type& oldVal, const component_type& newVal) : oldValue(oldVal), newValue(newVal), entity(entity) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }

    };

    template<typename component_type>
    struct component_destruction : public event<component_destruction<component_type>>
    {
        ecs::entity_handle entity;

        component_destruction() = default;
        component_destruction(ecs::entity_handle entity) : entity(entity) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };
}
