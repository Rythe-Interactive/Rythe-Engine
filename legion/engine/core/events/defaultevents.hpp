#pragma once
#include <core/events/event.hpp>
#include <core/ecs/handles/entity.hpp>

namespace legion::core::events
{
    struct exit final : public event<exit>
    {
        const int exitcode;

        exit(int exitcode = 0);
    };

    template<typename component_type>
    struct component_creation : public event<component_creation<component_type>>
    {
        ecs::entity entity;

        component_creation() = default;
        component_creation(ecs::entity entity) : entity(entity) {}
    };

    template<typename component_type>
    struct component_destruction : public event<component_destruction<component_type>>
    {
        ecs::entity entity;

        component_destruction() = default;
        component_destruction(ecs::entity entity) : entity(entity) {}
    };
}
