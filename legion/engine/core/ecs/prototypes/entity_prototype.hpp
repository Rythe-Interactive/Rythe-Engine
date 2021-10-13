#pragma once
#include <unordered_set>
#include <memory>

#include <core/serialization/prototype.hpp>

#include <core/ecs/data/entity_data.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>

/**
 * @file entity_prototype.hpp
 */

namespace legion::core::serialization
{
    /**@struct prototype<ecs::entity>
     * @brief Prototype specialization for entities.
     */
    template<>
    struct prototype<ecs::entity> : public prototype_base
    {
        std::string name;
        bool active;
        std::vector<prototype<ecs::entity>> children;
        std::unordered_map<id_type, std::unique_ptr<component_prototype_base>> composition;

        prototype(ecs::entity src);
        prototype() = default;
        prototype(const prototype&);
        prototype(prototype&&) = default;

        prototype& operator=(ecs::entity);
        prototype& operator=(const prototype&);
        prototype& operator=(prototype&&) = default;
    };


    /**@struct entity_prototype
     * @brief Type alias for `prototype<ecs::entity>`. Can be used to serialize and deserialize entities.
     */
    using entity_prototype = prototype<ecs::entity>;
}
