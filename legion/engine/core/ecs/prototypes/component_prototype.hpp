#pragma once
#include <unordered_set>

#include <core/serialization/prototype.hpp>
#include <core/types/reflector.hpp>

#include <core/ecs/handles/component.hpp>

/**
 * @file component_prototype.hpp
 */

namespace legion::core::serialization
{
    /**@struct prototype<ecs::component_base>
     * @brief Prototype specialization base class for component specializations.
     */
    template<>
    struct prototype<ecs::component_base> : public prototype_base
    {
        L_NODISCARD virtual std::unique_ptr<prototype<ecs::component_base>> copy() LEGION_PURE;
    };

    /**@struct prototype<ecs::component<component_type>>
     * @brief Prototype specialization components. Makes use of reflectors.
     * @ref legion::core::reflector
     */
    template<typename component_type>
    struct prototype<ecs::component<component_type>> : public prototype<ecs::component_base>, public decltype(make_reflector(std::declval<component_type>()))
    {
        using Reflector = decltype(make_reflector(std::declval<component_type>()));

        prototype() = default;
        prototype(const component_type& src) : Reflector(make_reflector(src)) {}
        prototype(component_type&& src) : Reflector(make_reflector(src)) {}

        L_NODISCARD virtual std::unique_ptr<prototype<ecs::component_base>> copy()
        {
            return std::make_unique<prototype<ecs::component<component_type>>>(*this);
        };

    };

    /**@class component_prototype_base
     * @brief Type alias for `prototype<ecs::component_base>`. Base class for component specializations.
     */
    using component_prototype_base = prototype<ecs::component_base>;

    /**@class component_prototype
     * @brief Type alias for `prototype<ecs::component<component_type>>`. Can be used to serialize and deserialize components.
     */
    template<typename component_type>
    using component_prototype = prototype<ecs::component<component_type>>;
}
