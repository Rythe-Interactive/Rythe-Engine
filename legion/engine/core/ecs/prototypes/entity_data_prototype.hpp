#pragma once
#include <unordered_set>
#include <memory>

#include <core/serialization/prototype.hpp>

#include <core/ecs/data/entity_data.hpp>
#include <core/ecs/prototypes/component_prototype.hpp>

/**
 * @file entity_data_prototype.hpp
 */

namespace legion::core::serialization
{
    /**@struct prototype<ecs::entity_data>
     * @brief Prototype specialization for entity_data.
     */
    template<>
    struct prototype<ecs::entity_data> : public prototype_base, public decltype(make_reflector(std::declval<ecs::entity_data>()))
    {
        using Reflector = decltype(make_reflector(std::declval<ecs::entity_data>()));

        prototype(const ecs::entity& src) : Reflector(make_reflector(src.data)) {}
        prototype(const ecs::entity_data*& data) : Reflector(make_reflector(data)) {}
        prototype() = default;
        prototype(const prototype& src);
        prototype(prototype&& src) = default;

        prototype& operator=(ecs::entity);
        prototype& operator=(const prototype&);
        prototype& operator=(prototype&&) = default;
    };


    /**@struct entity_data_prototype
     * @brief Type alias for `prototype<ecs::entity_data>`. is used by entity_prototype to serialize entity data.
     */
    using entity_data_prototype = prototype<ecs::entity_data>;
}
