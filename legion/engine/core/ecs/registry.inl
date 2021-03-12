#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename component_type, typename ...Args>
    inline component_pool<component_type>* Registry::tryEmplaceFamily(Args && ...args)
    {
        if (m_componentFamilies.count(make_hash<component_type>())) // Check and fetch in order to avoid a possibly unnecessary allocation and deletion.
            return static_cast<component_pool<component_type>*>(m_componentFamilies.at(make_hash<component_type>()).get());

        // Allocate and emplace if no item was found.
        return static_cast<component_pool<component_type>*>(
            m_componentFamilies.emplace(
                make_hash<component_type>(),
                std::unique_ptr<component_pool_base>(new component_pool<component_type>(std::forward<Args>(args)...))
            ).first->second.get() // std::pair<iterator, bool>.first --> iterator<std::pair<key, value>>->second --> std::unique_ptr.get() --> component_pool_base* 
            );
    }

    template<typename component_type, typename... Args>
    inline void ecs::Registry::registerComponentType(Args&&... args)
    {
        m_componentFamilies.emplace(
            make_hash<component_type>(),
            std::unique_ptr<component_pool_base>(new component_pool<component_type>(std::forward<Args>(args)...))
        );
    }

    template<typename component_type, typename... Args>
    inline component_pool<component_type>* ecs::Registry::getFamily(Args&&... args)
    {
        return tryEmplaceFamily<component_type>(std::forward<Args>(args)...);
    }

    template<typename component_type>
    inline component_type& Registry::createComponent(entity target)
    {
        tryEmplaceFamily<component_type>();
        entityCompositions().at(target).insert(make_hash<component_type>());
        FilterRegistry::markComponentAdd<component_type>(target);
        return component_pool<component_type>::create_component_direct(target);
    }

    template<typename component_type>
    inline component_type& Registry::createComponent(entity target, const serialization::component_prototype<component_type>& prototype)
    {
        tryEmplaceFamily<component_type>();
        entityCompositions().at(target).insert(make_hash<component_type>());
        FilterRegistry::markComponentAdd<component_type>(target);
        return component_pool<component_type>::create_component_direct(target, prototype);
    }

    template<typename component_type>
    inline component_type& Registry::createComponent(entity target, serialization::component_prototype<component_type>&& prototype)
    {
        tryEmplaceFamily<component_type>();
        entityCompositions().at(target).insert(make_hash<component_type>());
        FilterRegistry::markComponentAdd<component_type>(target);
        return component_pool<component_type>::create_component_direct(target, std::move(prototype));
    }

    template<typename component_type>
    inline void Registry::destroyComponent(entity target)
    {
        tryEmplaceFamily<component_type>();
        entityCompositions().at(target).erase(make_hash<component_type>());
        FilterRegistry::markComponentErase<component_type>(entity{ &Registry::entityData(target) });
        component_pool<component_type>::destroy_component_direct(target);
    }

    template<typename component_type>
    inline bool Registry::hasComponent(entity target)
    {
        tryEmplaceFamily<component_type>();
        return component_pool<component_type>::contains_direct(target);
    }

    template<typename component_type>
    inline component_type& Registry::getComponent(entity target)
    {
        tryEmplaceFamily<component_type>();
        return component_pool<component_type>::get_component_direct(target);
    }

}
