#include <core/ecs/registry.hpp>
#pragma once

namespace legion::core::ecs
{
    template<typename ComponentType, typename... Args>
    inline L_ALWAYS_INLINE void Registry::registerComponentType(Args&&... args)
    {
        auto& [typeId, componentType] = *(componentTypes().try_emplace(typeHash<ComponentType>(), std::make_unique<component_type<ComponentType>>(std::forward<Args>(args)...)).first);
        if (initialized())
        {
            tryEmplaceFamily(typeId, componentType->create_pool());
        }
    }

    template<typename ComponentType0, typename ComponentType1, typename... ComponentTypes, typename... Args>
    inline L_ALWAYS_INLINE void Registry::registerComponentType(Args&&... args)
    {
        registerComponentType<ComponentType0>(std::forward<Args>(args)...);
        registerComponentType<ComponentType1, ComponentTypes...>(std::forward<Args>(args)...);
    }

    template<typename ComponentType, typename... Args>
    inline L_ALWAYS_INLINE component_pool<ComponentType>* Registry::getFamily(Args&&... args)
    {
        id_type typeId = typeHash<ComponentType>();
        auto& families = getFamilies();
        if (families.count(typeId)) // Check and fetch in order to avoid a possibly unnecessary allocation and deletion.
            return dynamic_cast<component_pool<ComponentType>*>(families.at(typeId).get());

        // Allocate and emplace if no item was found.
        auto& [_, componentType] = *(componentTypes().try_emplace(typeId, std::make_unique<component_type<ComponentType>>(std::forward<Args>(args)...)).first);
        return dynamic_cast<component_pool<ComponentType>*>(families.emplace(
            typeId,
            std::move(componentType->create_pool())
        ).first->second.get()); // std::pair<iterator, bool>.first --> iterator<std::pair<key, value>>->second --> std::unique_ptr.get() --> component_pool_base*
    }

    template<typename ComponentType>
    inline L_ALWAYS_INLINE component_ref_t<ComponentType> Registry::createComponent(entity target)
    {
        if constexpr (is_archetype_v<ComponentType>)
        {
            return ComponentType::create(target);
        }
        else
        {
            id_type typeId = typeHash<ComponentType>();

            // Check and emplace component family if it doesn't exist yet.
            static bool checked = false; // Prevent unnecessary unordered_map lookups.
            if (!checked && !getFamilies().count(typeId))
            {
                checked = true;
                registerComponentType<ComponentType>();
            }

            // Update entity composition.
            instance.m_entityCompositions.at(target).insert(typeId);
            // Update filters.
            FilterRegistry::markComponentAdd<ComponentType>(target);
            // Actually create and return the component. (this uses the direct function which avoids use of virtual indirection)
            return *reinterpret_cast<ComponentType*>(getFamily(typeId)->create_component(target));
        }
    }

    namespace detail
    {
        template<typename ComponentType>
        inline L_ALWAYS_INLINE auto _create_component_ref(entity target)
        {
            if constexpr (is_archetype_v<ComponentType>)
            {
                return Registry::createComponent<ComponentType>(target);
            }
            else
            {
                return std::ref(Registry::createComponent<ComponentType>(target));
            }
        }
    }

    template<typename ComponentType0, typename ComponentType1, typename... ComponentTypeN>
    inline L_ALWAYS_INLINE component_ref_tuple<ComponentType0, ComponentType1, ComponentTypeN...> Registry::createComponent(entity target)
    {
        return std::make_tuple(detail::_create_component_ref<ComponentType0>(target), detail::_create_component_ref<ComponentType1>(target), detail::_create_component_ref<ComponentTypeN>(target)...);
    }

    template<typename ComponentType>
    inline L_ALWAYS_INLINE component_ref_t<ComponentType> Registry::createComponent(entity target, ComponentType&& value)
    {
        if constexpr (is_archetype_v<ComponentType>)
        {
            return ComponentType::create(target, std::forward<ComponentType>(value));
        }
        else
        {
            id_type typeId = typeHash<ComponentType>();

            // Check and emplace component family if it doesn't exist yet.
            static bool checked = false; // Prevent unnecessary unordered_map lookups.
            if (!checked && !getFamilies().count(typeId))
            {
                checked = true;
                registerComponentType<ComponentType>();
            }

            // Update entity composition.
            instance.m_entityCompositions.at(target).insert(typeId);
            // Update filters.
            FilterRegistry::markComponentAdd<ComponentType>(target);
            // Actually create and return the component.
            return reinterpret_cast<component_pool<ComponentType>*>(getFamily(typeId))->create_component(target, std::forward<ComponentType>(value));
        }
    }

    template<typename ComponentType>
    inline L_ALWAYS_INLINE component_ref_t<ComponentType> Registry::createComponent(entity target, const ComponentType& value)
    {
        if constexpr (is_archetype_v<ComponentType>)
        {
            return ComponentType::create(target, value);
        }
        else
        {
            id_type typeId = typeHash<ComponentType>();

            // Check and emplace component family if it doesn't exist yet.
            static bool checked = false; // Prevent unnecessary unordered_map lookups.
            if (!checked && !getFamilies().count(typeId))
            {
                checked = true;
                registerComponentType<ComponentType>();
            }

            // Update entity composition.
            instance.m_entityCompositions.at(target).insert(typeId);
            // Update filters.
            FilterRegistry::markComponentAdd<ComponentType>(target);
            // Actually create and return the component.
            return reinterpret_cast<component_pool<ComponentType>*>(getFamily(typeId))->create_component(target, value);
        }
    }

    template<typename archetype_type, typename ComponentType0, typename ComponentType1, typename... ComponentTypeN>
    inline L_ALWAYS_INLINE typename archetype_type::ref_group Registry::createComponent(entity target, ComponentType0&& value0, ComponentType1&& value1, ComponentTypeN&&... valueN)
    {
        return archetype_type::create(target, std::forward<ComponentType0>(value0), std::forward<ComponentType1>(value1), std::forward<ComponentTypeN>(valueN)...);
    }

    namespace detail
    {
        template<typename ComponentType>
        inline L_ALWAYS_INLINE auto _create_component_ref(entity target, ComponentType&& value)
        {
            if constexpr (is_archetype_v<ComponentType>)
            {
                return Registry::createComponent<remove_cvr_t<ComponentType>>(target, std::forward<ComponentType>(value));
            }
            else
            {
                return std::ref(Registry::createComponent<remove_cvr_t<ComponentType>>(target, std::forward<ComponentType>(value)));
            }
        }
    }

    template<typename ComponentType0, typename ComponentType1, typename... ComponentTypeN>
    inline L_ALWAYS_INLINE component_ref_tuple<ComponentType0, ComponentType1, ComponentTypeN...> Registry::createComponent(entity target, ComponentType0&& value0, ComponentType1&& value1, ComponentTypeN&&... valueN)
    {
        return std::make_tuple(detail::_create_component_ref(target, std::forward<ComponentType0>(value0)),
            detail::_create_component_ref(target, std::forward<ComponentType1>(value1)),
            detail::_create_component_ref(target, std::forward<ComponentTypeN>(valueN))...);
    }

    template<typename ComponentType0, typename ComponentType1, typename... ComponentTypeN>
    inline L_ALWAYS_INLINE component_ref_tuple<ComponentType0, ComponentType1, ComponentTypeN...> Registry::createComponent(entity target, const ComponentType0& value0, const ComponentType1& value1, const ComponentTypeN&... valueN)
    {
        return std::make_tuple(detail::_create_component_ref(target, value0),
            detail::_create_component_ref(target, value1),
            detail::_create_component_ref(target, valueN)...);
    }

    template<typename ComponentType>
    inline L_ALWAYS_INLINE void Registry::destroyComponent(entity target)
    {
        if constexpr (is_archetype_v<ComponentType>)
        {
            ComponentType::destroy(target);
        }
        else
        {
            id_type typeId = typeHash<ComponentType>();

            // Check and emplace component family if it doesn't exist yet.
            static bool checked = false; // Prevent unnecessary unordered_map lookups.
            if (!checked && !getFamilies().count(typeId))
            {
                checked = true;
                registerComponentType<ComponentType>();
            }

            // Update entity composition.
            instance.m_entityCompositions.at(target).erase(typeId);
            // Update filters.
            FilterRegistry::markComponentErase<ComponentType>(entity{ &Registry::entityData(target) });
            // Actually destroy the component.
            getFamily(typeId)->destroy_component(target);
        }
    }

    namespace detail
    {
        template<typename ComponentType>
        inline L_ALWAYS_INLINE void _destroy_comp_impl(entity target)
        {
            Registry::destroyComponent<ComponentType>(target);
        }

        template<typename ComponentType0, typename ComponentType1,  typename... ComponentTypeN>
        inline L_ALWAYS_INLINE void _destroy_comp_impl(entity target)
        {
            Registry::destroyComponent<ComponentType0>(target);
            _destroy_comp_impl<ComponentType1, ComponentTypeN...>(target);
        }
    }

    template<typename ComponentType0, typename ComponentType1, typename... ComponentTypeN>
    inline L_ALWAYS_INLINE void Registry::destroyComponent(entity target)
    {
        detail::_destroy_comp_impl<ComponentType0, ComponentType1, ComponentTypeN...>(target);
    }

    template<typename ComponentType>
    inline L_ALWAYS_INLINE bool Registry::hasComponent(entity target)
    {
        if constexpr (is_archetype_v<ComponentType>)
        {
            return ComponentType::has(target);
        }
        else
        {
            id_type typeId = typeHash<ComponentType>();

            // Check and emplace component family if it doesn't exist yet.
            static bool checked = false; // Prevent unnecessary unordered_map lookups.
            if (!checked && !getFamilies().count(typeId))
            {
                checked = true;
                registerComponentType<ComponentType>();
            }
            // Check if a component is existent.
            return getFamily(typeId)->contains(target);
        }
    }

    template<typename ComponentType0, typename ComponentType1, typename... ComponentTypeN>
    inline L_ALWAYS_INLINE bool Registry::hasComponent(entity target)
    {
        return (hasComponent<ComponentType0>(target) && hasComponent<ComponentType1>(target) && (hasComponent<ComponentTypeN>(target) && ...));
    }


    template<typename ComponentType>
    inline L_ALWAYS_INLINE component_ref_t<ComponentType> Registry::getComponent(entity target)
    {
        if constexpr (is_archetype_v<ComponentType>)
        {
            return ComponentType::get(target);
        }
        else
        {
            id_type typeId = typeHash<ComponentType>();

            // Check and emplace component family if it doesn't exist yet.
            static bool checked = false; // Prevent unnecessary unordered_map lookups.
            if (!checked && !getFamilies().count(typeId))
            {
                checked = true;
                registerComponentType<ComponentType>();
            }

            // Fetch the component.
            return *reinterpret_cast<ComponentType*>(getFamily(typeId)->get_component(target));
        }
    }

    namespace detail
    {
        template<typename ComponentType>
        inline L_ALWAYS_INLINE auto _get_component_ref(entity target)
        {
            if constexpr (is_archetype_v<ComponentType>)
            {
                return Registry::getComponent<remove_cvr_t<ComponentType>>(target);
            }
            else
            {
                return std::ref(Registry::getComponent<remove_cvr_t<ComponentType>>(target));
            }
        }
    }

    template<typename ComponentType0, typename ComponentType1, typename... ComponentTypeN>
    inline L_ALWAYS_INLINE component_ref_tuple<ComponentType0, ComponentType1, ComponentTypeN... > getComponent(entity target)
    {
        return std::make_tuple(detail::_get_component_ref<ComponentType0>(target), detail::_get_component_ref<ComponentType1>(target), detail::_get_component_ref<ComponentTypeN>(target)...);
    }
}
