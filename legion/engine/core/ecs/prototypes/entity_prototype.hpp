#pragma once
#include <unordered_set>
#include <memory>

#include <core/serialization/prototype.hpp>

#include <core/ecs/data/hierarchy.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>

namespace legion::core::serialization
{
    template<>
    struct prototype<ecs::entity> : public prototype_base
    {
        std::vector<prototype<ecs::entity>> children;
        std::unordered_map<id_type, std::unique_ptr<component_prototype_base>> composition;

        prototype() = default;
        prototype(const prototype&);
        prototype(prototype&&) = default;

        prototype& operator=(const prototype&);
        prototype& operator=(prototype&&) = default;
    };

    using entity_prototype = prototype<ecs::entity>;
}

#include <core/ecs/prototypes/entity_prototype.inl>
