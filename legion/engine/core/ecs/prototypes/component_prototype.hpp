#pragma once
#include <unordered_set>

#include <core/ecs/handles/component.hpp>

/**
 * @file component_prototype.hpp
 */

namespace legion::core::serialization
{
    /**@struct prototype<ecs::component_base>
     * @brief Prototype specialization base class for component specializations.
     */

    /**@struct prototype<ecs::component<component_type>>
     * @brief Prototype specialization components. Makes use of reflectors.
     * @ref legion::core::reflector
     */


    /**@class component_prototype_base
     * @brief Type alias for `prototype<ecs::component_base>`. Base class for component specializations.
     */


    /**@class component_prototype
     * @brief Type alias for `prototype<ecs::component<component_type>>`. Can be used to serialize and deserialize components.
     */

}
