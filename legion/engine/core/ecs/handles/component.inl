#include <core/ecs/handles/component.hpp>
#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type>
    inline component<component_type>::operator component_type& ()
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline component<component_type>::operator const component_type& () const
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline bool component<component_type>::valid() const noexcept
    {
		return Registry::hasComponent<component_type>(owner);
	}

    template<typename component_type>
    inline component<component_type>::operator bool() const noexcept
    {
        return Registry::hasComponent<component_type>(owner);
    }

    template<typename component_type>
    inline component_type* component<component_type>::operator->()
    {
        return &Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline const component_type* component<component_type>::operator->() const
    {
        return &Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline bool component<component_type>::operator==(const component& other) const noexcept
    {
        return owner == other.owner;
    }

    template<typename component_type>
    inline component_type& component<component_type>::get()
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline const component_type& component<component_type>::get() const
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline void component<component_type>::destroy()
    {
        Registry::destroyComponent<component_type>(owner);
    }

}
