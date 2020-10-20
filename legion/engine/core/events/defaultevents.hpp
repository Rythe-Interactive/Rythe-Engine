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
    struct component_destruction : public event<component_destruction<component_type>>
    {
        ecs::entity_handle entity;

        component_destruction() = default;
        component_destruction(ecs::entity_handle entity) : entity(entity) {}

        virtual bool persistent() override { return false; }
        virtual bool unique() override { return false; }
    };
}
