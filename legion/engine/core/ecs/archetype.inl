namespace legion::core::ecs
{
    template<typename component_type, typename ...component_types>
    inline std::tuple<component_handle<component_type>, component_handle<component_types>...> archetype<component_type, component_types...>::create(EcsRegistry* registry, id_type entityId)
    {
        return registry->createComponents<component_type, component_types...>(entityId);
    }

    template<typename component_type, typename ...component_types>
    inline std::tuple<component_handle<component_type>, component_handle<component_types>...> archetype<component_type, component_types...>::create(EcsRegistry* registry, id_type entityId, component_type&& defaultValue, component_types&&... defaultValues)
    {
        return registry->createComponents<component_type, component_types...>(entityId, std::forward<component_type>(defaultValue), std::forward<component_types>(defaultValues)...);
    }

    template<typename component_type, typename ...component_types>
    inline std::tuple<component_handle<component_type>, component_handle<component_types>...> archetype<component_type, component_types...>::get(EcsRegistry* registry, id_type entityId)
    {
        return registry->getComponents<component_type, component_types...>(entityId);
    }

    template<typename component_type, typename ...component_types>
    inline void archetype<component_type, component_types...>::destroy(EcsRegistry* registry, id_type entityId)
    {
        registry->destroyComponents<component_type, component_types...>(entityId);
    }

    template<typename component_type, typename ...component_types>
    inline bool archetype<component_type, component_types...>::has(EcsRegistry* registry, id_type entityId)
    {
        return registry->hasComponents<component_type, component_types...>(entityId);
    }

}
