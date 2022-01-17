#include <core/ecs/handles/component.hpp>
#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type>
    const type_hash component<component_type>::typeId = make_hash<component_type>();

    template<typename component_type>
    inline L_ALWAYS_INLINE component<component_type>& component<component_type>::operator=(const component_type& src)
    {
        Registry::getComponent<component_type>(owner) = src;
        return *this;
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component<component_type>& component<component_type>::operator=(component_type&& src)
    {
        Registry::getComponent<component_type>(owner) = std::move(src);
        return *this;
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component<component_type>::operator component_type& ()
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component<component_type>::operator const component_type& () const
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE bool component<component_type>::valid() const noexcept
    {
        return Registry::hasComponent<component_type>(owner);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component<component_type>::operator bool() const noexcept
    {
        return Registry::hasComponent<component_type>(owner);
    }

    template<typename component_type>
    L_NODISCARD component_type& component<component_type>::operator*()
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    L_NODISCARD const component_type& component<component_type>::operator*() const
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type* component<component_type>::operator->()
    {
        return &Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE const component_type* component<component_type>::operator->() const
    {
        return &Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE bool component<component_type>::operator==(const component& other) const noexcept
    {
        return owner == other.owner;
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE component_type& component<component_type>::get()
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE const component_type& component<component_type>::get() const
    {
        return Registry::getComponent<component_type>(owner);
    }

    template<typename component_type>
    inline L_ALWAYS_INLINE void component<component_type>::destroy()
    {
        Registry::destroyComponent<component_type>(owner);
    }

}
